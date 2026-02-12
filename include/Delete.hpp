#ifndef DELETE_METHOD_HPP
#define DELETE_METHOD_HPP

#include "Request.hpp"
#include "ConfigParser.hpp"
#include <string>

namespace webserv {
	namespace http {
		class Delete {
			public:
				// main function de del execute la methode http
				static std::string execute(const webserv::http::Request& req, const ServerConfig& server, const RouteConfig& route);
			
			private:
				// calcul le path a del et check les droit
				static std::string _getDeletePath(const std::string& reqPath, const ServerConfig& server, const RouteConfig& route, int& httpCode);	
				
				// del le ficher 
				static bool _deleteFile(const std::string& path);
		};

	}
}

#endif