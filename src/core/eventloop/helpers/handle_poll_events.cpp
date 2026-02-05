#include <cstring>
#include <cerrno>
#include <string>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "EventLoop.hpp"

using namespace webserv;

void webserv::core::EventLoop::_handle_poll_events()
{
	for (size_t i = 0; i < _poll_fds.size(); ++i) {

		if (_poll_fds[i].revents == 0) continue;

		int fd = _poll_fds[i].fd;
		short revents = _poll_fds[i].revents;

		std::map<int, webserv::core::Client>::iterator it_client = _clients.find(fd);
		bool is_known_client = (it_client != _clients.end());

		if (is_known_client) {
			 it_client->second.updateLastActivity();
		}

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

			if (is_listener)
				_accept_new_connection(fd);
			else
				_handle_client_data(fd, i);
		}

		if (revents & POLLOUT) {
			if (is_known_client && it_client->second.hasResponseToSend()) {
				
				std::string& data = it_client->second.getResponseBuffer();
				
				ssize_t bytes = ::send(fd, data.c_str(), data.size(), 0);
				
				if (bytes < 0) {
					_logger << "[EventLoop] Send error: " << std::strerror(errno) << std::endl;
					_close_connection(fd, i);
					i--;
					continue;
				} 
				else {
					data.erase(0, bytes);
					
					if (data.empty()) {
						_poll_fds[i].events = POLLIN; 

						// Pour l'instant : fermeture connexion (HTTP/1.0)
						// Plus tard ici : if (!keep_alive) close...
						_logger << "[EventLoop] Response sent fully to " << fd << std::endl;
						_close_connection(fd, i);
						i--;
						continue;
					}
				}
			}
		}
	}
}