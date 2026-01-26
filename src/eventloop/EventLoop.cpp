#include "../../include/EventLoop.hpp"
#include "../../lib/LIBFTPP/include/Net.hpp"

#include <iostream>
#include <cstring>
#include <cerrno>
#include <sstream>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>



using namespace libftpp::net;
using namespace webserv;

	EventLoop::EventLoop(const std::vector<int>& listen_sockets, const NetworkConfig& config) 
		: _logger("EventLoop"), _config(config), _listen_sockets(listen_sockets) {
		_setup_initial_poll_fds();
	}

	EventLoop::~EventLoop() {
		for (size_t i = 0; i < _poll_fds.size(); ++i) {
			if (_poll_fds[i].fd >= 0)
				close(_poll_fds[i].fd);
		}
	}

	void EventLoop::_setup_initial_poll_fds() {
		for (size_t i = 0; i < _listen_sockets.size(); ++i) {
			struct pollfd pfd;
			pfd.fd = _listen_sockets[i];
			pfd.events = POLLIN;
			pfd.revents = 0;
			_poll_fds.push_back(pfd);
		}
	}

	void EventLoop::run() {
		_logger << "[EventLoop] Entering main loop with " << _poll_fds.size() << " monitored fds." << std::endl;

		while (true) {
			int ret = poll(&_poll_fds[0], _poll_fds.size(), -1);
			
			if (ret < 0) {
				if (errno == EINTR) continue;
				_logger << "[EventLoop] poll error: " << strerror(errno) << std::endl;
				break;
			}

			if (ret > 0) {
				_handle_poll_events();
			}
		}
	}

	void EventLoop::_handle_poll_events() {
		
		for (size_t i = 0; i < _poll_fds.size(); ++i) {
			if (_poll_fds[i].revents == 0) continue;

			int fd = _poll_fds[i].fd;
			short revents = _poll_fds[i].revents;

			if (revents & (POLLERR | POLLHUP | POLLNVAL)) {
				 _close_connection(fd, i);
				 i--;
				 continue;
			}

			if (revents & POLLIN) {
				bool is_listener = false;
				for (size_t j = 0; j < _listen_sockets.size(); ++j) {
					if (fd == _listen_sockets[j]) {
						is_listener = true;
						break;
					}
				}

				if (is_listener) {
					_accept_new_connection(fd);
				} else {
					_handle_client_data(fd, i);
				}
			}
		}
	}

	void EventLoop::_accept_new_connection(int listen_fd) {
		struct sockaddr_in client_addr;
		socklen_t addr_len = sizeof(client_addr);
		
		int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &addr_len);
		if (client_fd < 0) {
			_logger << "[EventLoop] accept failed: " << strerror(errno) << std::endl;
			return;
		}

		if (!libftpp::net::set_non_blocking(client_fd)) {
			_logger << "[EventLoop] Failed to set client non-blocking" << std::endl;
			close(client_fd);
			return;
		}

		struct pollfd pfd;
		pfd.fd = client_fd;
		pfd.events = POLLIN;
		pfd.revents = 0;
		_poll_fds.push_back(pfd);

		_client_parsers[client_fd] = http::RequestParser();

		_logger << "[EventLoop] New connection accepted (fd: " << client_fd << ")" << std::endl;
	}

	void EventLoop::_handle_client_data(int client_fd, size_t poll_index) {
		char buffer[4096];
		ssize_t bytes = read(client_fd, buffer, sizeof(buffer));

		if (bytes < 0) {
			_logger << "[EventLoop] read error on fd " << client_fd << ": " << strerror(errno) << std::endl;
			_close_connection(client_fd, poll_index);
		} else if (bytes == 0) {
			_logger << "[EventLoop] Client disconnected (fd: " << client_fd << ")" << std::endl;
			_close_connection(client_fd, poll_index);
		} else {
			http::RequestParser& parser = _client_parsers[client_fd];
			http::RequestParser::State state = parser.parse(buffer, bytes);
			if (state == http::RequestParser::COMPLETE) {
				_logger << "[EventLoop] Request complete on fd " << client_fd << std::endl;
				http::Request& req = parser.getRequest();
				_logger << "Method: " << req.getMethod() << " Path: " << req.getPath() << std::endl;
				
				std::string responseData;

				switch (EventLoop::toEnum(req.getMethod())) {
					case GET:
						responseData=runGetMethod(parser.getRequest());
						break;
					case DELET:
						responseData=runDeletMethod(parser.getRequest());
						break;
					case POST:
						responseData=runPostMethod(parser.getRequest());
						break;
					case ERROR:
					{
						std::string body = "<html><body><h1>501 Not Implemented</h1></body></html>";
						std::ostringstream oss;
						oss << "HTTP/1.1 501 Not Implemented\r\n";
						oss << "Content-Type: text/html\r\n";
						oss << "Content-Length: " << body.length() << "\r\n";
						oss << "\r\n";
						oss << body;
						responseData = oss.str();
						_logger << "Method: " << req.getMethod() << " not supported" << std::endl;
					}
				}

				if (!responseData.empty())
					send(client_fd, responseData.c_str(), responseData.length(), 0);
				
				// actuellement je ferme après une requête (HTTP/1.0) (plus simple)
				// si 1.1 reset le parser pour keep-alive (HTTP/1.1) plus tard
				_close_connection(client_fd, poll_index);

			} else if (state == http::RequestParser::ERROR) {
				_logger << "[EventLoop] Parsing error on fd " << client_fd << std::endl;
				_close_connection(client_fd, poll_index);
			}
			// Si PARSING, on attend les prochaines données sans rien faire
		}
	}

	void EventLoop::_close_connection(int fd, size_t poll_index) {
		close(fd);
	
		if (poll_index < _poll_fds.size() - 1) {
			_poll_fds[poll_index] = _poll_fds.back();
		}
		_poll_fds.pop_back();
		_client_parsers.erase(fd);
		_logger << "[EventLoop] Connection closed (fd: " << fd << ")" << std::endl;
	}