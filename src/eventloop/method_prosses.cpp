#include "../../include/EventLoop.hpp"
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>

using namespace webserv;

// ====== actuellement ======
// 1. Chercher ressource demandée -> req.getPath()
// simule une réponse 200 OK en hardcode

//  ----------------------------------------------------------------------------

// Si tout va bien -> 200 OK
// Si fichier non trouvé -> 404 Not Found
// Si permission fichier refusée -> 403 Forbidden
// Si GET interdit dans la config -> 405 Method Not Allowed

//  ----------------------------------------------------------------------------

//1. Construire le chemin complet (Root + URL).
//2. Est-ce que ça existe ? 
//   NON -> 404.
//   OUI -> Est-ce un dossier ?
//		  OUI -> Chercher fichier Index ou générer Autoindex.
//		  NON -> C'est un fichier.
//3. A-t-on les droits de lecture ?
//   NON -> 403.
//4. Déterminer le type MIME (ex: .html -> text/html).
//5. Lire le fichier dans un buffer.
//6. Construire la réponse (Headers + Buffer) et renvoyer 200 OK.

//  ----------------------------------------------------------------------------

std::string EventLoop::runGetMethod(const http::Request &req, const ServerConfig& config)
{
	std::string root = config.root; 
	
	std::string fullPath = root + req.getPath();

	struct stat s;
	if (::stat(fullPath.c_str(), &s) != 0) {
		std::string body = "<html><body><h1>404 Not Found</h1></body></html>";
		std::ostringstream response;
		response << "HTTP/1.1 404 Not Found\r\nContent-Length: " << body.length() << "\r\n\r\n" << body;
		return response.str();
	}

	if (s.st_mode & S_IFDIR) {
		if (fullPath[fullPath.size() - 1] != '/') fullPath += "/";
		fullPath += "index.html";

		if (::stat(fullPath.c_str(), &s) != 0) {
			std::string body = "<html><body><h1>403 Forbidden (No Index)</h1></body></html>";
			std::ostringstream response;
			response << "HTTP/1.1 403 Forbidden\r\nContent-Length: " << body.length() << "\r\n\r\n" << body;
			return response.str();
		}
	}

	if (::access(fullPath.c_str(), R_OK) != 0) {
		std::string body = "<html><body><h1>403 Forbidden</h1></body></html>";
		std::ostringstream response;
		response << "HTTP/1.1 403 Forbidden\r\nContent-Length: " << body.length() << "\r\n\r\n" << body;
		return response.str();
	}

	std::string content = readFile(fullPath);
	std::string mimeType = getContentType(fullPath);

	std::ostringstream response;
	response << "HTTP/1.1 200 OK\r\n";
	response << "Content-Type: " << mimeType << "\r\n";
	response << "Content-Length: " << content.length() << "\r\n";
	response << "Connection: close\r\n"; 
	response << "\r\n";
	response << content;

	return response.str();
}

std::string EventLoop::runDeletMethod(const http::Request &req, const ServerConfig& config)
{
	(void)req;
	(void)config;
	std::string body = "<html><body><h1>DELETE method</h1></body></html>";

	std::ostringstream response;
	response << "HTTP/1.1 200 OK\r\n";
	response << "Content-Type: text/html\r\n";
	response << "Content-Length: " << body.length() << "\r\n";
	response << "\r\n";
	response << body;
	return response.str();
}

std::string EventLoop::runPostMethod(const http::Request &req, const ServerConfig& config)
{
	(void)req;
	(void)config;
	std::string body = "<html><body><h1>POST method</h1></body></html>";

	std::ostringstream response;
	response << "HTTP/1.1 200 OK\r\n";
	response << "Content-Type: text/html\r\n";
	response << "Content-Length: " << body.length() << "\r\n";
	response << "\r\n";
	response << body;
	return response.str();
}

