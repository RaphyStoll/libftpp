#include <fstream>
#include <sstream>
#include <climits>
#include <cstdlib>
#include <vector>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "Post.hpp"
#include "ResponseBuilder.hpp"
#include "libftpp.hpp"
#include "Request.hpp"
#include "RouteMatcher.hpp"

using namespace webserv::http;

std::string webserv::http::Post::execute(const webserv::http::Request& req, const ServerConfig& config, const RouteConfig& route)
{
	libftpp::debug::DebugLogger _logger("post");
	int httpCode = 200;
	(void)httpCode; // FIXME : pas use pour l'instant mais comme la fontion n'est pas fini je le laisse (pareil sur get)

	std::string effectiveRoute = webserv::http::RouteMatcher::getEffectiveRoot(config, route);
	std::string fullPath = libftpp::str::PathUtils::join(effectiveRoute, req.getPath());

	_logger << "Post effectiveRoute: " << effectiveRoute << std::endl;
	_logger << "Post fullPath: " << fullPath << std::endl;

	if (fullPath.find(effectiveRoute) != 0)
		return _logger << fullPath << " find " << effectiveRoute << " failed" << std::endl,
			ResponseBuilder::generateError(403, config);
	
	if (libftpp::str::PathUtils::isDirectory(fullPath)) {
		
		std::string indexName;
		if (!config.index.empty()) {
			_logger << "Index from config: " << config.index << std::endl;
			indexName = config.index;
		} 
		else {
			_logger << "Index is empty, Fallback to index.html" << std::endl;
			indexName = "index.html"; 
		}

		std::string indexPath = libftpp::str::PathUtils::join(fullPath, indexName);
		
		if (libftpp::str::PathUtils::exists(indexPath)) {
			_logger << "Index found: " << indexPath << std::endl;
			fullPath = indexPath; 
		} else {
			return _logger << "No index found for POST on directory" << std::endl,
				ResponseBuilder::generateError(403, config);
		}
	}

	if (route.cgi == true) {
		std::string ext = route.cgi_extension;
		
		if (fullPath.length() >= ext.length() && 
			fullPath.compare(fullPath.length() - ext.length(), ext.length(), ext) == 0) 
		{
			_logger << "===== CGI (POST) =====" << std::endl;
			_logger << "cgi = " << req.getPath() << std::endl;

			// TODO SDU: cgi ici Call
			// std::string output = execute_cgi(req, config, 0); 
			// return output;
			
			return "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nCGI POST OUTPUT (TODO)"; 
		}
	}

	if (req.getBody().size() > config.max_body_size) {
		return ResponseBuilder::generateError(413, config);
	}
	_logger << "Writing to file: " << fullPath << std::endl;
	
	std::ofstream file(fullPath.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
	if (!file.is_open())
		return _logger << "Could not open file for writing: " << fullPath << std::endl,
			ResponseBuilder::generateError(500, config);

	file.write(req.getBody().c_str(), req.getBody().size());
	file.close();

	int responseCode = 201;
	std::ostringstream response;
	response << "HTTP/1.1 " << responseCode << " " << ResponseBuilder::getStatusMessage(responseCode) << "\r\n";
	if (responseCode == 201) {
		response << "Location: " << req.getPath() << "\r\n";
	}
	response << "Content-Length: 0\r\n";
	response << "Connection: close\r\n\r\n";
	
    return response.str();
}