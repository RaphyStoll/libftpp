#include "RouteMatcher.hpp"

bool webserv::http::RouteMatcher::isMethodAllowed(const std::string& method, const RouteConfig& route)
{
	if (route.methods.empty()) {
		return true; 
	}

	for (size_t i = 0; i < route.methods.size(); ++i) {
		if (route.methods[i] == method) {
			return true;
		}
	}

	return false;
}
