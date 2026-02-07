#ifndef ROUTE_MATCHER_HPP
#define ROUTE_MATCHER_HPP

#include <string>

#include "ConfigParser.hpp"

namespace webserv {
	namespace http {

		class RouteMatcher {
			public:
				// cherche la bonne route
				// meme logique que nginx "Longest Prefix Match" on prend le path le plus long
				// ex : images/vacances/photo.jpg dans config j'ai /, /images, /images/vacances, /images/vacances/2025
				// mathc / -> ok
				// match /images -> ok
				// match /images/vacances -> ok
				// match /images/vacances/2025 -> ko
			
				// plus long match = /images/vacances
				static const RouteConfig& findRoute(const std::string& reqPath, const ServerConfig& server);
			
				// check if méthode (GET, POST...) est autoriser dans cette route
				static bool isMethodAllowed(const std::string& method, const RouteConfig& route);
			
				// Renvoie la bonne racine (Route root > Server root)
				static std::string getEffectiveRoot(const ServerConfig& server, const RouteConfig& route);
		}; 

	}
}

#endif