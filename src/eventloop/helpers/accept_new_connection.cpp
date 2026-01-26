#include "../../../include/EventLoop.hpp"
#include "../../../lib/LIBFTPP/include/Net.hpp"

#include <iostream>
#include <cstring>
#include <cerrno>
#include <sstream>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>



using namespace libftpp::net;
using namespace webserv;

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