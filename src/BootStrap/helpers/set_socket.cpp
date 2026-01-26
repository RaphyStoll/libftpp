#include "../../../include/BootStrap.hpp"
#include "../../../lib/LIBFTPP/include/libftpp.hpp"

#include <cstring>
#include <stdexcept>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

using namespace libftpp::net;
using namespace libftpp::str;
using namespace webserv;

void BootStrap::_setup_sockets() 
	{
		NetworkConfig::const_iterator it;
		for (it = _config.begin(); it != _config.end(); ++it) {
			int port = it->first;

			if (it->second.empty()) continue; 

			std::string host = it->second.front().listen;
			if (host.empty())
				host = "0.0.0.0";
			
			try {
				int sock_fd = create_listener_socket(port, host);
				_listen_sockets.push_back(sock_fd);

				std::cout << "Listening on http://" << host << ":" << port << std::endl;
				_logger << "[BootStrap] Listening on http://" << host << ":" << port << " (fd: " << sock_fd << ")" << std::endl;
			} catch (const std::exception& e) {
				std::cerr << "Error: Could not bind port " << port << ": " << e.what() << std::endl;
				_logger << "[BootStrap] Failed to bind port " << port << ": " << e.what() << std::endl;
			}
		}
	}