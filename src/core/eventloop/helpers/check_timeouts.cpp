#include "EventLoop.hpp"

using namespace webserv;

void webserv::core::EventLoop::_check_timeouts() {
	unsigned long long now = libftpp::time::Clock::now_ms();

	for (size_t i = 0; i < _poll_fds.size(); ) {
		int fd = _poll_fds[i].fd;

		std::map<int, webserv::core::Client>::iterator it = _clients.find(fd);

		if (it != _clients.end()) {
			webserv::core::Client& client = it->second;

			// On passe(60s) comme limite hardcodée pour l'instant.
			// (after passer 'config.keepalive_timeout')
			if (client.hasTimedOut(now, 60000)) {
				_logger << "[EventLoop] Timeout expired for fd " << fd << " - Closing connection." << std::endl;
				_close_connection(fd, i);
				continue; 
			}
		}

		i++;
	}
}