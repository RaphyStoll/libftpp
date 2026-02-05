#include "Get.hpp"
#include "ResponseBuilder.hpp"

using namespace webserv::http;

bool webserv::http::Get::_checkIndexFile(std::string& fullPath, int& httpCode) {
	if (fullPath[fullPath.size() - 1] != '/') fullPath += "/";
	
	std::string indexPath = fullPath + "index.html"; // TODO RAPH: Utiliser config.index
	struct stat s;
	if (stat(indexPath.c_str(), &s) == 0) {
		fullPath = indexPath;
		return true;
	}
	httpCode = 403;
	return false;
}