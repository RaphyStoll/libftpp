#include "ResponseBuilder.hpp"
#include "Get.hpp"

#include <iostream>
#include <sstream>
# include <string>

using namespace webserv::http;

std::string webserv::http::ResponseBuilder::build(const ::http::Request& req, const ServerConfig& config) {
	
	std::string method = req.getMethod();

	//   http://37.59.120.163:9003/login2.html   //SDU
	req.print();//SDU

	if (method == "GET") {
		return Get::execute(req, config);
	} 
	else if (method == "POST") {
		return std::cout << "POST METHOD" << std::endl, "POST"; //Post::execute(req, config);
	} 
	else if (method == "DELETE") {
		return std::cout << "DELETE METHOD" << std::endl, "DELETE"; //Delete::execute(req, config);
	}
	
	// Méthode inconnue ou non gérée
	return generateError(501, config);
}

std::string webserv::http::ResponseBuilder::generateError(int code, const ServerConfig& config) {
	std::string msg = getStatusMessage(code);
	std::string body;
	bool customPageFound = false;

	std::map<int, std::string>::const_iterator it = config.error_pages.find(code);
	if (it != config.error_pages.end()) {
		std::string filePath = config.root + it->second;
		
		// Si le chemin dans config commence par '/', on le concatène (ou pas selon ta logique de parsing)
		// Ici je suppose que it->second est "/404.html" et qu'on doit le chercher dans la root.
		// Si ton parser gère déjà le path absolu, adapte cette ligne :
		if (it->second[0] != '/') filePath = config.root + "/" + it->second;
		else filePath = config.root + it->second;

		struct stat s;
		if (::stat(filePath.c_str(), &s) == 0 && !(s.st_mode & S_IFDIR) && ::access(filePath.c_str(), R_OK) == 0) {
			body = _readFile(filePath);
			if (!body.empty()) {
				customPageFound = true;
			}
		}
	}

	// Fallback
	if (!customPageFound) {
		std::ostringstream oss;
		oss << "<!DOCTYPE html><html><head><title>" << code << " " << msg << "</title></head>";
		oss << "<body style=\"font-family: sans-serif; text-align: center; padding: 50px;\">";
		oss << "<h1>Error " << code << "</h1>";
		oss << "<h2>" << msg << "</h2>";
		oss << "<hr><p>webserv/1.0</p></body></html>";
		body = oss.str();
	}

	std::ostringstream response;
	response << "HTTP/1.1 " << code << " " << msg << "\r\n";
	response << "Content-Type: text/html\r\n";
	response << "Content-Length: " << body.length() << "\r\n";
	response << "Connection: close\r\n";
	response << "\r\n";
	response << body;

	return response.str();
}

// =============================================================================
// 3. HELPERS
// =============================================================================



