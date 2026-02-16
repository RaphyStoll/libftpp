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
	
	// Si le préfixe de la requête correspond à la route, on ne veut probablement pas 
	// le dupliquer dans le path physique final si 'root' est un alias.
	// MAIS dans la structure actuelle, getPath() retourne le path complet de la requête (ex: /post/newfile.html).
	// Et effectiveRoute retourne './www/upload'.
	// PathUtils::join concatène betement : ./www/upload/post/newfile.html
	// Ce qui est incorrect si on veut mapper /post -> ./www/upload
	
	std::string reqPath = req.getPath();
	std::string fullPath;

	// Tentative de gestion type "alias" basique :
	// Si la route match est "/post" et reqPath est "/post/newfile",
	// et que root est "./www/upload", on veut "./www/upload/newfile".
	// Donc on doit retirer la partie "route path" du "req path" avant de join.

	// Attention: route.path n'est pas passé directement à execute, mais est dans 'route'.
	// On suppose que 'route' est la config de la route matchée.
	
	if (reqPath.find(route.path) == 0) {
		std::string suffix = reqPath.substr(route.path.length());
		fullPath = libftpp::str::PathUtils::join(effectiveRoute, suffix);
	} else {
		// Cas fallback si jamais ça match pas (peu probable si RouteMatcher a bien fait son taff)
		fullPath = libftpp::str::PathUtils::join(effectiveRoute, reqPath);
	}

	_logger << "Post effectiveRoute: " << effectiveRoute << std::endl;
	_logger << "Post reqPath: " << reqPath << std::endl;
	_logger << "Post routePath: " << route.path << std::endl;
	_logger << "Post fullPath (fixed): " << fullPath << std::endl;

/*
	std::string fullPath = libftpp::str::PathUtils::join(effectiveRoute, req.getPath());

	_logger << "Post effectiveRoute: " << effectiveRoute << std::endl;
	_logger << "Post fullPath: " << fullPath << std::endl;

	if (fullPath.find(effectiveRoute) != 0)
		return _logger << fullPath << " find " << effectiveRoute << " failed" << std::endl,
			ResponseBuilder::generateError(403, config);
*/
	if (libftpp::str::PathUtils::isDirectory(fullPath)) {
		if (route.cgi == false) {
			// TODO: Pour l'instant, on rejette le POST sur un dossier sans CGI.
			return _logger << "POST on directory without CGI forbidden" << std::endl,
				ResponseBuilder::generateError(403, config);
		}
	}
/*
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
*/
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