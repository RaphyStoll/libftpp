# pragma once

# include "../lib/LIBFTPP/include/libftpp.hpp"
# include "BootStrap.hpp"
# include "EventLoop.hpp"

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
		_logger << "[BootStrap] Starting server initialization..." << std::endl;

		try {
			_setup_sockets();
		} catch (const std::exception& e) {
			_logger << "[BootStrap] Error during socket setup: " << e.what() << std::endl;
			throw; 
		}

		_logger << "[BootStrap] Sockets ready. Launching EventLoop..." << std::endl;

		// TODO: Ici EventLoop
		EventLoop loop(_listen_sockets);
		loop.run();
	}

	template <typename TConfig>
	void BootStrap<TConfig>::_setup_sockets() 
	{
		typename TConfig::const_iterator it;
		for (it = _config.begin(); it != _config.end(); ++it) {
			int port = it->first;

			int sock_fd = create_listener_socket(port);
			_listen_sockets.push_back(sock_fd);

			// Récupération de l'hôte depuis la première configuration serveur pour ce port
			std::string host = it->second.front().listen;
			if (host.empty())
				host = "0.0.0.0";
				
			std::cout << "Listening on http://" << host << ":" << port << " (fd: " << sock_fd << ")" << std::endl;
		}
	}
}


