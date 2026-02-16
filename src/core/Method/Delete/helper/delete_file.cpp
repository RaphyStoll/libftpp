#include "Delete.hpp"
#include "ResponseBuilder.hpp"
#include "RouteMatcher.hpp"
#include "libftpp.hpp"

#include <sstream>
#include <unistd.h>
#include <cstdio>

using namespace webserv::http;

bool webserv::http::Delete::_deleteFile(const std::string& path, libftpp::debug::DebugLogger _logger) {
    _logger << "Attempting to delete file: " << path << std::endl;
    if (std::remove(path.c_str()) == 0) {
        _logger << "File successfully deleted." << std::endl;
        return true;
    }
    _logger << "Failed to delete file." << std::endl;
    return false;
}