#include "../../../include/EventLoop.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <cstring>
#include <cerrno>
#include <string>

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
			}
			else {
				_handle_client_data(fd, i);
			}
		}
		if (revents & POLLOUT) {
            std::map<int, std::string>::iterator it = _write_buffers.find(fd);
            if (it != _write_buffers.end() && !it->second.empty()) {
                std::string& data = it->second;
                
                // On essaie d'envoyer tout le buffer
                ssize_t bytes = ::send(fd, data.c_str(), data.size(), 0);
                
                if (bytes < 0) {
                    _logger << "[EventLoop] Send error: " << std::strerror(errno) << std::endl;
                    _close_connection(fd, i);
                    i--; // Index reculé car suppression
                    continue;
                } 
                else {
                    // On retire ce qui a été envoyé (gestion du "Short Write")
                    data.erase(0, bytes);
                    
                    // Si tout est envoyé
                    if (data.empty()) {
                        _write_buffers.erase(it);
                        
                        // Pour l'instant on ferme après réponse (HTTP/1.0 style)
                        _logger << "[EventLoop] Response sent fully to " << fd << std::endl;
                        _close_connection(fd, i);
                        i--; // Index reculé
                        continue;
                    }
				}
				
			}
		}
	}
}