#include "../include/BootStrap.hpp"
#include "../include/Config.hpp"
#include "../include/ConfigParser.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace webserv;

//tmp
NetworkConfig parseConfig(const std::string& config_file) {
	(void)config_file; //argv qui est le pathg du cofig file
	NetworkConfig config;

	ServerConfig1 srv1;
	srv1.server_name = "localhost";
	srv1.root = "/var/www/html";
	config[9001].push_back(srv1);

	ServerConfig1 srv2;
	srv2.server_name = "admin";
	srv2.root = "/var/www/admin";
	config[9002].push_back(srv2);

	return config;
}

int main(int argc, char** argv) {
	DataConfig data;
	data.config_path = "config/config.conf";

	if (argc > 1) {
		data.config_path = argv[1];
	}
	try {
	
		std::cout << "[Main] Loading configuration from " << data.config_path << "..." << std::endl;
		openFileAndParseConfig(&data);
		NetworkConfig net_config = parseConfig(data.config_path);

		std::cout << "[Main] Config loaded. Initializing BootStrap..." << std::endl;
		BootStrap<NetworkConfig> server(net_config);

		server.start();

	} catch (const std::exception& e) {
		std::cerr << "Fatal Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}