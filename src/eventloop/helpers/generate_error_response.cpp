#include "../../../include/EventLoop.hpp"
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>

using namespace webserv;

std::string EventLoop::_generateErrorResponse(int code, const std::string& msg, const ServerConfig& config) {
    std::string body;
    bool customPageFound = false;

    std::map<int, std::string>::const_iterator it = config.error_pages.find(code);
    if (it != config.error_pages.end()) {
        std::string filePath = it->second;
        struct stat s;

        if (::stat(filePath.c_str(), &s) == 0 && !(s.st_mode & S_IFDIR) && ::access(filePath.c_str(), R_OK) == 0) {
            body = readFile(filePath);
            if (!body.empty()) {
                customPageFound = true;
            }
        }
    }

    // Fallback html minimal
    if (!customPageFound) {
        std::ostringstream oss;
        oss << "<html><head><title>" << code << " " << msg << "</title></head>";
        oss << "<body><center><h1>" << code << " " << msg << "</h1></center>";
        oss << "<hr><center>webserv/1.0</center></body></html>";
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
