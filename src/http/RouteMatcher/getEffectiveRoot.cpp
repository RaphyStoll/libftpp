#include "RouteMatcher.hpp"

std::string webserv::http::RouteMatcher::getEffectiveRoot(const ServerConfig& server, const RouteConfig& route)
{
	if (!route.root.empty())
		return route.root;

	return server.root;
}