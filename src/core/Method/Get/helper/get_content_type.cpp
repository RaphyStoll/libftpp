#include "Get.hpp"
#include "ResponseBuilder.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace webserv::http;


std::string webserv::http::Get::_getContentType(const std::string& path) {
	size_t dotPos = path.rfind('.');
	if (dotPos == std::string::npos) return "text/plain";

	std::string ext = path.substr(dotPos);
	
	if (ext == ".html" || ext == ".htm") return "text/html";
	if (ext == ".css") return "text/css";
	if (ext == ".js") return "application/javascript";
	if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
	if (ext == ".png") return "image/png";
	if (ext == ".gif") return "image/gif";
	if (ext == ".ico") return "image/x-icon";
	if (ext == ".txt") return "text/plain";
	if (ext == ".mp4") return "video/mp4";
	
	return "application/octet-stream";
}