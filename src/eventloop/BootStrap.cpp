#include "../../include/BootStrap.hpp"
#include "../../lib/LIBFTPP/include/libftpp.hpp"

#include <cstring>
#include <stdexcept>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

using namespace libftpp::net;
using namespace libftpp::str;


using namespace webserv;

	BootStrap::BootStrap(const NetworkConfig& par_config) : _logger("BootStrap"), _config(par_config) {}

	void BootStrap::start() {
		_logger << "[BootStrap] Starting server initialization..." << std::endl;

		try {
			_setup_sockets();
		} catch (const std::exception& e) {
			_logger << "[BootStrap] Error during socket setup: " << e.what() << std::endl;
			throw; 
		}

		_logger << "[BootStrap] Sockets ready." << std::endl;
	}

	void BootStrap::_setup_sockets() 
	{
		NetworkConfig::const_iterator it;
		for (it = _config.begin(); it != _config.end(); ++it) {
			int port = it->first;

			int sock_fd = create_listener_socket(port);
			_listen_sockets.push_back(sock_fd);

			std::string host = it->second.front().listen;
			if (host.empty())
				host = "0.0.0.0";
				
				std::cout << "Listening on http://" << host << ":" << port << std::endl;
				_logger << "[BootStrap] Listening on http://" << host << ":" << port << " (fd: " << sock_fd << ")" << std::endl;
			}
	}

	const std::vector<int>& BootStrap::getListenSockets() const {
		return _listen_sockets;
	}

	int create_listener_socket(int port, const std::string& host) {
	int sockfd;
	struct sockaddr_in addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		throw std::runtime_error("socket() failed: ");
	}

	if (!set_reuseaddr(sockfd)) {
		close(sockfd);
		throw std::runtime_error("set_reuseaddr() failed");
	}

	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0) {
		close(sockfd);
		throw std::runtime_error("Invalid IP address: " + host);
	}
	addr.sin_port = htons(port);

	if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		close(sockfd);
		throw std::runtime_error("bind() failed on port " + StringUtils::itos(port));
	}

	if (listen(sockfd, SOMAXCONN) < 0) {
		close(sockfd);
		throw std::runtime_error("listen() failed");
	}

	if (!set_non_blocking(sockfd)) {
		close(sockfd);
		throw std::runtime_error("set_non_blocking() failed");
	}
	return sockfd;
}
