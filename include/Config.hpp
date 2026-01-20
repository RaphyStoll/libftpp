#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include <map>

namespace webserv {

	// Structure représentant la configuration d'un serveur virtuel (bloc 'server' dans nginx)
	struct ServerConfig1 {
		std::string server_name;
		std::string root;
		// Ajoutez ici d'autres directives (error_pages, client_max_body_size, etc.)
	};

	// La configuration réseau est une map : Port -> Liste de ServerConfig
	// Cela permet de gérer plusieurs serveurs sur le même port (virtual hosting)
	typedef std::map<int, std::vector<ServerConfig1> > NetworkConfig;

}

#endif
