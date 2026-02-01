#include "../../../include/EventLoop.hpp"

using namespace webserv;

void EventLoop::_check_timeouts() {
    unsigned long long now = libftpp::time::Clock::now_ms();

    for (size_t i = 0; i < _poll_fds.size(); ) {
        int fd = _poll_fds[i].fd;

        if (_client_timeouts.find(fd) != _client_timeouts.end()) {
            
            if (_client_timeouts[fd].expired(now)) {
                _logger << "[EventLoop] Timeout expired for fd " << fd << " - Closing connection." << std::endl;

                _close_connection(fd, i);

                continue; 
            }
        }

        i++;
    }
}