#include "../../include/BootStrap.hpp"
#include "../../lib/LIBFTPP/include/libftpp.hpp"

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

	BootStrap::BootStrap(const NetworkConfig& par_config) : _logger("BootStrap"), _config(par_config) {}

	void BootStrap::start() {
		_logger << "[BootStrap] Starting server initialization..." << std::endl;

		try {
			_setup_sockets();
		} catch (const std::exception& e) {
			_logger << "[BootStrap] Error during socket setup: " << e.what() << std::endl;
			throw; 
		}

		_logger << "[BootStrap] Sockets ready." << std::endl;
	}

	

	const std::vector<int>& BootStrap::getListenSockets() const {
		return _listen_sockets;
	}

	