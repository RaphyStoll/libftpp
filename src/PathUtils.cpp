#include "PathUtils.hpp"
#include "libftpp.hpp" 
#include <sys/stat.h>
#include <unistd.h>
#include <string>

using namespace libftpp::str;

std::string PathUtils::join(const std::string& root, const std::string& reqPath) {
    std::vector<std::string> parts = StringUtils::split(reqPath, '/');
    std::vector<std::string> resolved;

    for (size_t i = 0; i < parts.size(); ++i) {
        if (parts[i] == "" || parts[i] == ".") continue;
        if (parts[i] == "..") {
            if (!resolved.empty()) resolved.pop_back();
        } else {
            resolved.push_back(parts[i]);
        }
    }

    std::string cleanPath = root;
    if (!cleanPath.empty() && cleanPath[cleanPath.size() - 1] == '/')
        cleanPath.erase(cleanPath.size() - 1);

    for (size_t i = 0; i < resolved.size(); ++i) {
        cleanPath += "/" + resolved[i];
    }

    return cleanPath;
}

bool PathUtils::exists(const std::string& path) {
    return (access(path.c_str(), F_OK) == 0);
}

bool PathUtils::isDirectory(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) return false;
    return S_ISDIR(st.st_mode);
}