#ifndef POST_HPP
#define POST_HPP

#include "Request.hpp"
#include "ConfigParser.hpp"
#include <string>

namespace webserv {
	namespace http {
	
		class Post {
		public:
			static std::string execute(const ::http::Request& req, const ServerConfig& config);
		
		private:
		
			// Vérifie la taille du body vs config
			static bool _checkBodySize(size_t bodySize, size_t maxSize, int& httpCode);
		
			// Calcule le chemin de destination et vérifie qu'on a le droit d'écrire
			static std::string _getUploadPath(const std::string& root, const std::string& reqPath, int& httpCode);
		
			// Écrit physiquement le fichier
			static bool _writeFile(const std::string& path, const std::string& content);
		};
	
	} // namespace http
} // namespace webserv

#endif