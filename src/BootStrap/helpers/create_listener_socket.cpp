#include "../../../include/BootStrap.hpp"
#include "../../../lib/LIBFTPP/include/libftpp.hpp"

#include <cstring>
#include <cerrno>
#include <stdexcept>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>


using namespace libftpp::net;
using namespace libftpp::str;
using namespace webserv;

int BootStrap::_create_listener_socket(int port, const std::string& host) {
	int sockfd;
	struct sockaddr_in addr;
	std::string err;

	sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		err = ::strerror(errno);
		_logger << "[BootStrap] socket() failed on port " << host << ":" << StringUtils::itos(port) << err << std::endl;
		throw std::runtime_error("socket() failed: ");
	}

	if (!set_reuseaddr(sockfd)) {
		err = ::strerror(errno);
		::close(sockfd);
		_logger << "[BootStrap] set_reuseaddr() failed on port " << host << ":" << StringUtils::itos(port) << err << std::endl;
		throw std::runtime_error("set_reuseaddr() failed");
	}

	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	if (::inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0) {
		err = ::strerror(errno);
		::close(sockfd);
		_logger << "[BootStrap] inet_pton() failed on port " << host << ":" << StringUtils::itos(port) << err << std::endl;
		throw std::runtime_error("Invalid IP address: " + host);
	}
	addr.sin_port = htons(port);

	printf("%s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));//SDU
	//	_logger << inet_ntoa(addr.sin_addr) << " : " << ntohs(addr.sin_port) << std::endl;//SDU

	if (::bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		err = ::strerror(errno);
		::close(sockfd);
		_logger << "[BootStrap] bind() failed on port " << host << ":" << StringUtils::itos(port) << " errno = " << err << std::endl;
		throw std::runtime_error("bind() failed on port " + StringUtils::itos(port));
	}

	if (::listen(sockfd, SOMAXCONN) < 0) {
		err = ::strerror(errno);
		::close(sockfd);
		_logger << "[BootStrap] listen() failed on port " << host << ":" << StringUtils::itos(port) << err << std::endl;
		throw std::runtime_error("listen() failed");
	}

	if (!set_non_blocking(sockfd)) {
		err = ::strerror(errno);
		::close(sockfd);
		_logger << "[BootStrap] set_non_blocking() failed on port " << host << ":" << StringUtils::itos(port) << err << std::endl;
		throw std::runtime_error("set_non_blocking() failed");
	}
	return sockfd;
}
