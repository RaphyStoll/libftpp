#include "../../../include/EventLoop.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace webserv;

// Détermine le type MIME en fonction de l'extension
std::string EventLoop::_getContentType(const std::string& path) {
	if (path.rfind(".html") != std::string::npos) return "text/html";
	if (path.rfind(".css") != std::string::npos) return "text/css";
	if (path.rfind(".js") != std::string::npos) return "application/javascript";
	if (path.rfind(".jpg") != std::string::npos || path.rfind(".jpeg") != std::string::npos) return "image/jpeg";
	if (path.rfind(".png") != std::string::npos) return "image/png";
	return "text/plain";
}