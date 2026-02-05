#include "Get.hpp"
#include "ResponseBuilder.hpp"

#include <string>
#include <climits>
#include <cstdlib>

using namespace webserv::http;

std::string webserv::http::Get::_getSecurePath(const std::string& root, const std::string& reqPath, int& httpCode) {
	char resolvedRoot[PATH_MAX];
	if (realpath(root.c_str(), resolvedRoot) == NULL) {
		httpCode = 500; // Root invalide config serveur
		return "";
	}

	std::string rawPath = root + reqPath;
	char resolvedPath[PATH_MAX];
	
	// Si le chemin n'existe pas
	if (realpath(rawPath.c_str(), resolvedPath) == NULL) {
		httpCode = 404;
		return "";
	}

	std::string absPath = resolvedPath;
	std::string absRoot = resolvedRoot;

	// Jail Check : Le chemin final DOIT commencer par la racine
	if (absPath.find(absRoot) != 0) {
		httpCode = 403; // Tentative de sortie de la racine
		return "";
	}

	return absPath;
}