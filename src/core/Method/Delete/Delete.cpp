#include "Delete.hpp"
#include "ResponseBuilder.hpp"
#include "RouteMatcher.hpp"
#include "libftpp.hpp"

#include <sstream>
#include <unistd.h>
#include <cstdio>

using namespace webserv::http;

std::string webserv::http::Delete::execute(const webserv::http::Request& req, const ServerConfig& server, const RouteConfig& route)
{

	int httpCode = 204;

	std::string fullPath = _getDeletePath(req.getPath(), server, route, httpCode);
	if (httpCode != 204) {
		return ResponseBuilder::generateError(httpCode, server);
	}
	if (!_deleteFile(fullPath)) {
		return ResponseBuilder::generateError(500, server);
	}
	std::ostringstream response;
    
	response << "HTTP/1.1 204 " << ResponseBuilder::getStatusMessage(204) << "\r\n";
	response << "Connection: close\r\n";
	response << "\r\n";

	return response.str();
}