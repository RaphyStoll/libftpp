#include "../include/BootStrap.hpp"
#include "../include/Config.hpp"
#include "../include/ConfigParser.hpp"
#include "../include/EventLoop.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace webserv;

//tmp
// NetworkConfig mapConfig(const std::string& config_file) {
// 	(void)config_file; //argv qui est le pathg du cofig file
// 	NetworkConfig config;

// 	ServerConfig1 srv1;
// 	srv1.server_name = "localhost";
// 	srv1.root = "/var/www/html";
// 	config[9001].push_back(srv1);

// 	ServerConfig1 srv2;
// 	srv2.server_name = "admin";
// 	srv2.root = "/var/www/admin";
// 	config[9002].push_back(srv2);

// 	return config;
// }

NetworkConfig mapConfig(DataConfig *data)
{
	NetworkConfig net_tmp;

	for(size_t i = 0; i < data->servers.size(); i++)
	{
		net_tmp[data->servers[i].port].push_back(data->servers[i]);
	}
 	return net_tmp;

}

int main(int argc, char** argv) {
	DataConfig data;
	libftpp::debug::DebugLogger _logger("Main");
	data.config_path = "config/config.conf";

	if (argc > 1) {
		data.config_path = argv[1];
	}
	try {

		_logger << "[Main] Loading configuration from " << data.config_path << "..." << std::endl;
		openFileAndParseConfig(&data);

		NetworkConfig net_config = mapConfig(&data);

		_logger << "[Main] Config loaded. Initializing BootStrap..." << std::endl;
		BootStrap server1(net_config);//

		server1.start();
		
		// Note: Il faudra ajouter la config au constructeur d'EventLoop
		std::cout << "server1.getListenSockets()[0] = " << server1.getListenSockets()[0] << std::endl;
		std::cout << "server1.getListenSockets()[1] = " << server1.getListenSockets()[1] << std::endl;
		std::cout << "server1.getListenSockets()[2] = " << server1.getListenSockets()[2] << std::endl;
		webserv::core::EventLoop loop(server1.getListenSockets(), net_config);
		loop.run();

	} catch (const std::exception& e) {
		std::cerr << "Fatal Error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}