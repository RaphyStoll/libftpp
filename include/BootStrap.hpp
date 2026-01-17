#ifndef BOOTSTRAP_HPP
# define BOOTSTRAP_HPP

# include <vector>
# include <iostream>

namespace webserv {

	/**
	 * @brief Classe Bootstrap
	 * 
	 * Elle prend un type TConfig
	 * Cela permet de changer la structure de la config (Map, Vector, List, ...
	 * sans avoir à modifier ce header
	 */
	template <typename TConfig>
	class BootStrap {
		public:
			BootStrap(const TConfig& config) : _config(config) {}
			~BootStrap() {}

			void start();

		private:
			TConfig _config;
			std::vector<int> _listen_sockets;
			void _setup_sockets();
	};

	int create_listener_socket(int port);
}

#include "BootStrap.tpp"

#endif
