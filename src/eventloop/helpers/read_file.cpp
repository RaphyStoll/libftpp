#include "../../../include/EventLoop.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace webserv;

// Lit tout le fichier dans une string (binaire compliant)
std::string EventLoop::readFile(const std::string& path) {
	std::ifstream file(path.c_str(), std::ios::binary);
	std::ostringstream ss;
	ss << file.rdbuf();
	return ss.str();
}

