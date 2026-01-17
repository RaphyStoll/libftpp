#include "../../include/BootStrap.hpp"
#include "../../lib/LIBFTPP/include/Net.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <cstring>
#include <stdexcept>

using namespace libftpp::net;

int webserv::create_listener_socket(int port) {
	int sockfd;
	struct sockaddr_in addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		throw std::runtime_error("socket() failed: " + std::string(strerror(errno)));
	}

	if (!set_reuseaddr(sockfd)) {
		close(sockfd);
		throw std::runtime_error("set_reuseaddr() failed");
	}

	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY; 
	addr.sin_port = htons(port);

	if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		close(sockfd);
		throw std::runtime_error("bind() failed on port " + std::to_string(port));
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
