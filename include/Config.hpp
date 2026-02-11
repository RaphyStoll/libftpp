#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include <map>
#include"ConfigParser.hpp"

namespace webserv {

	// Structure représentant la configuration d'un serveur virtuel (bloc 'server' dans nginx)
//SDU	struct ServerConfig1 {
//SDU		std::string server_name;
//SDU		std::string root;
		// Ajoutez ici d'autres directives (error_pages, client_max_body_size, etc.)
//SDU	};

	// La configuration réseau est une map : Port -> Liste de ServerConfig
	// Cela permet de gérer plusieurs serveurs sur le même port (virtual hosting)
//SDU	typedef std::map<int, std::vector<ServerConfig1> > NetworkConfig;

	typedef std::map<int, std::vector<ServerConfig> > NetworkConfig;



}

#endif
