#include "../include/BootStrap.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace webserv;

//tmp config
struct ServerConfig {
        std::string server_name;
        std::string root;
    };

    typedef std::map<int, std::vector<ServerConfig> > NetworkConfig;

//tmp
NetworkConfig parseConfig(const std::string& config_file) {
    (void)config_file; //argv qui est le pathg du cofig file
    NetworkConfig config;

    ServerConfig srv1;
    srv1.server_name = "localhost";
    srv1.root = "/var/www/html";
    config[8080].push_back(srv1);

    ServerConfig srv2;
    srv2.server_name = "admin";
    srv2.root = "/var/www/admin";
    config[9090].push_back(srv2);

    return config;
}

int main(int argc, char** argv) {
    std::string config_path = "config/default.conf";
    if (argc > 1) {
        config_path = argv[1];
    }

    try {
        std::cout << "[Main] Loading configuration from " << config_path << "..." << std::endl;
        NetworkConfig net_config = parseConfig(config_path);

        std::cout << "[Main] Config loaded. Initializing BootStrap..." << std::endl;
        BootStrap<NetworkConfig> server(net_config);

        server.start();

    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}