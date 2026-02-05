#include "Get.hpp"
#include "ResponseBuilder.hpp" 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace webserv::http;

// Lit tout le fichier dans une string (binaire compliant)
std::string webserv::http::Get::_readFile(const std::string& path) {
	std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
	if (!file.is_open()) return "";
	
	std::ostringstream ss;
	ss << file.rdbuf();
	return ss.str();
}