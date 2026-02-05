#include <string>
#include <iostream>
#include <cstring>
#include <cerrno>
#include <sstream>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "libftpp.hpp"
#include "EventLoop.hpp"
#include "Client.hpp" 
#include "Request.hpp" 
#include "RequestParser.hpp"
#include "ResponseBuilder.hpp"

using namespace libftpp::net;
using namespace webserv;

void webserv::core::EventLoop::_handle_client_data(int client_fd, size_t poll_index)
{
	char buffer[4096];
	ssize_t bytes = ::read(client_fd, buffer, sizeof(buffer));

	if (bytes < 0) {
		_logger << "[EventLoop] read error on fd " << client_fd << ": " << std::strerror(errno) << std::endl;
		_close_connection(client_fd, poll_index);
	}
	else if (bytes == 0) {
		_logger << "[EventLoop] Client disconnected (fd: " << client_fd << ")" << std::endl;
		_close_connection(client_fd, poll_index);
	}
	else {
		if (_clients.find(client_fd) == _clients.end()) return;
		webserv::core::Client& client = _clients.at(client_fd);

		::http::RequestParser& parser = client.getParser();
		::http::RequestParser::State state = parser.parse(buffer, bytes, webserv::core::EventLoop::getConfig());

		if (state ==http::RequestParser::COMPLETE) {
			_logger << "[EventLoop] Request complete on fd " << client_fd << std::endl;

			::http::Request& req = parser.getRequest();
			_logger << "Method: " << req.getMethod() << " Path: " << req.getPath() << std::endl;

			const ServerConfig& srvConfig = _getServerConfig(client_fd, req);

			std::string responseData = webserv::http::ResponseBuilder::build(req, srvConfig);

			if (!responseData.empty()) {
				client.appendResponse(responseData);
				_poll_fds[poll_index].events = POLLIN | POLLOUT;
			}
			
			// TODO RAPH: ici keep alive after est ici (reset parser)

		}
		else if (state ==http::RequestParser::ERROR) {
			_logger << "[EventLoop] Parsing error on fd " << client_fd << std::endl;
			_close_connection(client_fd, poll_index);
		}
	}
}