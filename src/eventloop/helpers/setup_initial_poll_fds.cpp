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

void EventLoop::_setup_initial_poll_fds() {
		for (size_t i = 0; i < _listen_sockets.size(); ++i) {
			struct pollfd pfd;
			pfd.fd = _listen_sockets[i];
			pfd.events = POLLIN;
			pfd.revents = 0;
			_poll_fds.push_back(pfd);
		}
	}