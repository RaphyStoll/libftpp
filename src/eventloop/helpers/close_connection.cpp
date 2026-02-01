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

void EventLoop::_close_connection(int fd, size_t poll_index) {
		close(fd);
	
		if (poll_index < _poll_fds.size() - 1) {
			_poll_fds[poll_index] = _poll_fds.back();
		}
		_poll_fds.pop_back();
		_client_parsers.erase(fd);
		_write_buffers.erase(fd);
		_logger << "[EventLoop] Connection closed (fd: " << fd << ")" << std::endl;
	}