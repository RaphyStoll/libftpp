# pragma once

# include "../lib/LIBFTPP/include/Net.hpp"
# include "BootStrap.hpp"

# include <sys/socket.h>
# include <netinet/in.h>
# include <fcntl.h>
# include <unistd.h>

# include <cstring>
# include <stdexcept>
# include <iostream>



namespace webserv {

	template <typename TConfig>
	void BootStrap<TConfig>::start() {
		std::cout << "[BootStrap] Starting server initialization..." << std::endl;

		try {
			_setup_sockets();
		} catch (const std::exception& e) {
			std::cerr << "[BootStrap] Error during socket setup: " << e.what() << std::endl;
			throw; 
		}

		std::cout << "[BootStrap] Sockets ready. Launching EventLoop..." << std::endl;

		// TODO: Ici viendra l'instanciation de l'EventLoop
		// EventLoop loop(_listen_sockets);
		// loop.run();
	}

	template <typename TConfig>
	void BootStrap<TConfig>::_setup_sockets() {
		typename TConfig::const_iterator it;
		for (it = _config.begin(); it != _config.end(); ++it) {
			int port = it->first;

			int sock_fd = create_listener_socket(port);
			_listen_sockets.push_back(sock_fd);

			std::cout << "[BootStrap] Listening on port " << port << " (fd: " << sock_fd << ")" << std::endl;
		}
	}
}


