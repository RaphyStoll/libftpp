#ifndef BOOTSTRAP_HPP
# define BOOTSTRAP_HPP

# include <vector>
# include <iostream>

# include "../lib/LIBFTPP/include/libftpp.hpp"
# include "Config.hpp"

namespace webserv {

	/**
	 * @brief Classe Bootstrap
	 * 
	 * Elle prend un type NetworkConfig
	 */
	class BootStrap
	{
		public:
			BootStrap(const NetworkConfig& par_config);
			~BootStrap() {}

			void start();
			const std::vector<int>& getListenSockets() const;

		private:
			libftpp::debug::DebugLogger _logger;
			NetworkConfig _config;
			std::vector<int> _listen_sockets;
			void _setup_sockets();
			int _create_listener_socket(int port, const std::string& host);
	};

}

#endif
