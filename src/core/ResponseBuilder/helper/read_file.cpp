#include <string>

#include "../../../../include/ResponseBuilder.hpp"

std::string webserv::http::ResponseBuilder::_readFile(const std::string& path) {
    std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
    if (!file.is_open()) return "";
    
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}