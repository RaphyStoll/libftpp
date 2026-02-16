#include "EventLoop.hpp"
#include "libftpp.hpp"

#include <iostream>
#include <cstring>
#include <cerrno>
#include <sstream>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>



using namespace libftpp::net;
using namespace webserv;

	webserv::core::EventLoop::EventLoop(const std::vector<int>& listen_sockets, const NetworkConfig& config) 
		: _logger("EventLoop"), _config(config), _listen_sockets(listen_sockets) {
		//_logger << "Initializing EventLoop with " << listen_sockets.size() << " listening sockets." << std::endl;
		_setup_initial_poll_fds();
	}

	webserv::core::EventLoop::~EventLoop() {
		//_logger << "Destroying EventLoop." << std::endl;
		for (size_t i = 0; i < _poll_fds.size(); ++i) {
			if (_poll_fds[i].fd >= 0)
				close(_poll_fds[i].fd);
		}
	}
	
	const NetworkConfig	webserv::core::EventLoop::getConfig()
	{
		return this->_config;
	}

	void webserv::core::EventLoop::run() {
		_logger << "[EventLoop] Entering main loop with " << _poll_fds.size() << " monitored fds." << std::endl;

		while (true) {
			int ret = poll(&_poll_fds[0], _poll_fds.size(), 1000);
			
			if (ret < 0) {
				if (errno == EINTR) continue;
				_logger << "[EventLoop] poll error: " << std::strerror(errno) << std::endl;
				break;
			}

			if (ret > 0) {
				_handle_poll_events();
			}
			_check_timeouts();
		}
	}





	