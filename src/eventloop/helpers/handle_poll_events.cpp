#include "../../../include/EventLoop.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace webserv;

void EventLoop::_handle_poll_events() {
		
	for (size_t i = 0; i < _poll_fds.size(); ++i) {
		if (_poll_fds[i].revents == 0) continue;

		int fd = _poll_fds[i].fd;
		short revents = _poll_fds[i].revents;

		if (revents & (POLLERR | POLLHUP | POLLNVAL)) {
			 _close_connection(fd, i);
			 i--;
			 continue;
		}

		if (revents & POLLIN) {
			bool is_listener = false;
			for (size_t j = 0; j < _listen_sockets.size(); ++j) {
				if (fd == _listen_sockets[j]) {
					is_listener = true;
					break;
				}
			}

			if (is_listener) {
				_accept_new_connection(fd);
			} else {
				_handle_client_data(fd, i);
			}
		}
	}
}