#include <sstream>

#include "EventLoop.hpp"

using namespace webserv;

const ServerConfig& webserv::core::EventLoop::_getServerConfig(int client_fd, const http::Request& req) {
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
	
	int port = 80; 
	if (getsockname(client_fd, (struct sockaddr *)&addr, &len) == 0) {
		port = ntohs(addr.sin_port);
	}

	if (_config.find(port) == _config.end()) {
		static ServerConfig default_cfg; 
		return default_cfg;
	}

	const std::vector<ServerConfig>& servers = _config.at(port);//SSDU

	std::string host_header = req.getHeader("Host");
	size_t colon_pos = host_header.find(':');
	if (colon_pos != std::string::npos) {
		host_header = host_header.substr(0, colon_pos);
	}

	if (!host_header.empty()) {
		for (std::vector<ServerConfig>::const_iterator it = servers.begin(); it != servers.end(); ++it) {
			if (it->server_name == host_header)
				return *it;
		}
	}
	return servers.front();
}

