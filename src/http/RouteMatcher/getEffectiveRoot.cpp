#include "RouteMatcher.hpp"

std::string webserv::http::RouteMatcher::getEffectiveRoot(const ServerConfig& server, const RouteConfig& route)
{
	libftpp::debug::DebugLogger _logger("RouteMatcher");
	if (!route.root.empty()) {
		//_logger << "Using route root: " << route.root << std::endl;
		return route.root;
	}

	//_logger << "Using server root: " << server.root << std::endl;
	return server.root;
}