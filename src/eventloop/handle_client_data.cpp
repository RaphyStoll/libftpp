#include <string>
#include <iostream>
#include <cstring>
#include <cerrno>
#include <sstream>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../../include/EventLoop.hpp"
#include "../../lib/LIBFTPP/include/Net.hpp"

using namespace libftpp::net;
using namespace webserv;

void EventLoop::_handle_client_data(int client_fd, size_t poll_index) {
	char buffer[4096];
	ssize_t bytes = ::read(client_fd, buffer, sizeof(buffer));

	if (bytes < 0) {
		_logger << "[EventLoop] read error on fd " << client_fd << ": " << std::strerror(errno) << std::endl;
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

			std::string responseData = NULL;

			// Sélection du bon Virtual Server
			const ServerConfig& srvConfig = _getServerConfig(client_fd, req);

			switch (EventLoop::_toEnum(req.getMethod())) {
				case GET:
					responseData = _runGetMethod(req, srvConfig);
					break;
				case DELETE:
					responseData = _runDeletMethod(req, srvConfig);
					break;
				case POST:
					responseData = _runPostMethod(req, srvConfig);
					break;
				case ERROR:
					responseData = _generateErrorResponse(501, "Not Implemented", srvConfig);
					_logger << "Method: " << req.getMethod() << " not supported" << std::endl;
					break;
			}

			if (!responseData.empty())
			{
				_write_buffers[client_fd] += responseData;
				_poll_fds[poll_index].events = POLLIN | POLLOUT;
				//::send(client_fd, responseData.c_str(), responseData.length(), 0);
			}
			
			// actuellement je ferme après une requête (HTTP/1.0) (plus simple)
			// si 1.1 reset le parser pour keep-alive (HTTP/1.1) plus tard

		} else if (state == http::RequestParser::ERROR) {
			_logger << "[EventLoop] Parsing error on fd " << client_fd << std::endl;
			_close_connection(client_fd, poll_index);
		}
			// Si PARSING, on attend les prochaines données sans rien faire
	}
}