#include "Get.hpp"
#include "ResponseBuilder.hpp"

using namespace webserv::http;

std::string webserv::http::Get::_createSuccessResponse(const std::string& content, const std::string& path) {
	std::string mimeType = _getContentType(path);
	std::ostringstream response;

	response << "HTTP/1.1 200 " << ResponseBuilder::getStatusMessage(200) << "\r\n";
	response << "Content-Type: " << mimeType << "\r\n";
	response << "Content-Length: " << content.length() << "\r\n";
	response << "Connection: close\r\n"; 
	response << "\r\n";
	response << content;

	return response.str();
}