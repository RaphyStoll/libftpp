#include "Get.hpp"
#include "ResponseBuilder.hpp"

#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/stat.h>
#include <climits>
#include <cstdlib>

using namespace webserv::http;

std::string webserv::http::Get::execute(const http::Request& req, const ServerConfig& config)
{
	int httpCode = 200;

	std::string fullPath = _getSecurePath(config.root, req.getPath(), httpCode);
	if (httpCode != 200)
		return ResponseBuilder::generateError(httpCode, config);

	struct stat s;
	if (stat(fullPath.c_str(), &s) == 0 && (s.st_mode & S_IFDIR)) {
		if (!_checkIndexFile(fullPath, httpCode)) {
			// TODO: RAPH
			// Ici, plus tard : quelque cgose comme if (config.autoindex) return AutoIndex::gen(...);
			return ResponseBuilder::generateError(httpCode, config);
		}
	}

	if (access(fullPath.c_str(), R_OK) != 0)
		return ResponseBuilder::generateError(403, config);

	std::string content = _readFile(fullPath);
	if (content.empty() && s.st_size > 0)
		return ResponseBuilder::generateError(500, config);

	return _createSuccessResponse(content, fullPath);
}
