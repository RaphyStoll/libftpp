#include "RouteMatcher.hpp"

bool webserv::http::RouteMatcher::isMethodAllowed(const std::string& method, const RouteConfig& route)
{
	libftpp::debug::DebugLogger _logger("RouteMatcher");
	
	if (route.methods.empty()) {
		_logger << "No methods restricted, " << method << " allowed by default." << std::endl;
		return true; 
	}

	for (size_t i = 0; i < route.methods.size(); ++i) {
		if (route.methods[i] == method) {
			_logger << "Method " << method << " allowed for route " << route.path << std::endl;
			return true;
		}
	}

	_logger << "Method " << method << " denied for route " << route.path << std::endl;
	return false;
}
