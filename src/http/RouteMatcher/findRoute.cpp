#include "RouteMatcher.hpp"

const RouteConfig& webserv::http::RouteMatcher::findRoute(const std::string& reqPath, const ServerConfig& server) {
	libftpp::debug::DebugLogger _logger("RouteMatcher");
	_logger << "Finding route for path: " << reqPath << std::endl;
	const std::vector<RouteConfig>& routes = server.routes;
	
	const RouteConfig* bestMatch = NULL;
	size_t bestMatchLength = 0;

	// On parcourt toutes les routes disponibles dans le serveur
	for (size_t i = 0; i < routes.size(); ++i) {
		const std::string& routePath = routes[i].path;
		size_t len = routePath.length();

		// 1. Prefix Match : Est-ce que la requête commence par le path de la route ?
		if (reqPath.find(routePath) == 0) {
			
			// 2. Boundary Check (Logique Nginx)
			// On veut éviter que "/img" match "/img_backup"
			// La route doit matcher exactement OU être suivie d'un '/'
			
			bool exactMatch = (reqPath.length() == len);
			bool subFolderMatch = (reqPath.length() > len && reqPath[len] == '/');
			// Cas spécial : La route "/" match tout
			bool rootMatch = (len == 1 && routePath[0] == '/');

			if (exactMatch || subFolderMatch || rootMatch) {
				
				// 3. Longest Match : On garde la plus longue trouvée
				if (bestMatch == NULL || len > bestMatchLength) {
					bestMatch = &routes[i];
					bestMatchLength = len;
				}
			}
		}
	}
	
	if (bestMatch) {
		_logger << "Best route match found: " << bestMatch->path << std::endl;
	} else {
		_logger << "No matching route found, fallback to default." << std::endl;
	}

	// Sécurité : Si aucune route ne correspond (théoriquement impossible si "/" existe)
	// On retourne la première route (souvent la default) ou on lance une exception.
	if (bestMatch == NULL && !routes.empty()) {
		return routes[0]; 
	}
	
	// Si bestMatch est NULL ici, c'est que la config est vide -> Crash ou Default
	return *bestMatch;
}