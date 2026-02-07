#include "Post.hpp"

#include <string>
bool webserv::http::Post::_writeFile(const std::string& path, const std::string& content)
{
	std::ofstream file(path.c_str(), std::ios::binary | std::ios::trunc);
	if (!file.is_open()) {
		return false;
	}

	file << content;
	if (file.fail()) {
		file.close();
		return false;
	}
	file.close();
	return true;
}
