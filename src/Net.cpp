#include "../include/Net.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>

#include <sstream>
#include <iomanip>

#include <netinet/tcp.h>
#include <netdb.h>
#include <netinet/in.h>

namespace libftpp {
namespace net {

bool set_non_blocking(int fd) {
	if (fd < 0) {
		errno = EBADF;
		return false;
	}
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return false;
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		return false;
	return true;
}

bool set_reuseaddr(int fd) {
	if (fd < 0) {
		errno = EBADF;
		return false;
	}
	int yes = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
		return false;
	return true;
}

bool set_nodelay(int fd) {
	if (fd < 0) {
		errno = EBADF;
		return false;
	}

	struct protoent* pe = getprotobyname("tcp");
	if (!pe)
		return false;

	int yes = 1;
	if (setsockopt(fd, pe->p_proto, TCP_NODELAY, &yes, sizeof(yes)) == -1)
		return false;
	return true;
}

static std::string ipv4_to_string_manual(const struct sockaddr_in* in, bool with_port) {
	unsigned long a = (unsigned long)ntohl(in->sin_addr.s_addr);

	unsigned int b1 = (unsigned int)((a >> 24) & 0xFF);
	unsigned int b2 = (unsigned int)((a >> 16) & 0xFF);
	unsigned int b3 = (unsigned int)((a >>  8) & 0xFF);
	unsigned int b4 = (unsigned int)((a >>  0) & 0xFF);

	std::ostringstream oss;
	oss << b1 << "." << b2 << "." << b3 << "." << b4;

	if (with_port) {
		oss << ":" << (unsigned int)ntohs(in->sin_port);
	}
	return oss.str();
}

static std::string ipv6_to_string_manual(const struct sockaddr_in6* in6, bool with_port) {
	const unsigned char* p = (const unsigned char*)(&in6->sin6_addr);

	std::ostringstream ip;
	ip << std::hex << std::setfill('0');

	for (int i = 0; i < 16; i += 2) {
		unsigned int group = ((unsigned int)p[i] << 8) | (unsigned int)p[i + 1];
		if (i != 0) ip << ":";
		ip << std::setw(4) << group;
	}

	std::ostringstream out;
	if (with_port) {
		out << "[" << ip.str() << "]:" << (unsigned int)ntohs(in6->sin6_port);
	} else {
		out << ip.str();
	}
	return out.str();
}

std::string sockaddr_to_string(const struct sockaddr* sa, socklen_t /*salen*/, bool with_port) {
	if (!sa)
		return std::string();

	if (sa->sa_family == AF_INET) {
		const struct sockaddr_in* in = (const struct sockaddr_in*)sa;
		return ipv4_to_string_manual(in, with_port);
	}
	if (sa->sa_family == AF_INET6) {
		const struct sockaddr_in6* in6 = (const struct sockaddr_in6*)sa;
		return ipv6_to_string_manual(in6, with_port);
	}

	errno = EAFNOSUPPORT;
	return std::string();
}

Fd::Fd() : _fd(-1) {}
Fd::Fd(int fd) : _fd(fd) {}

Fd::~Fd() {
	if (_fd >= 0) {
		::close(_fd);
		_fd = -1;
	}
}

bool Fd::valid() const { return _fd >= 0; }
int  Fd::get() const { return _fd; }

void Fd::reset(int fd) {
	if (_fd >= 0)
		::close(_fd);
	_fd = fd;
}

int Fd::release() {
	int tmp = _fd;
	_fd = -1;
	return tmp;
}

}
}
