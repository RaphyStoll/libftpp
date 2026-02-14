#include <fstream>
#include <sstream>
#include <climits>
#include <cstdlib>
#include <vector>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "Get.hpp"
#include "ResponseBuilder.hpp"
#include "libftpp.hpp"
#include "Request.hpp"
#include "RouteMatcher.hpp"

using namespace webserv::http;

std::string execute_cgi(const webserv::http::Request& req, const ServerConfig& config, size_t i)
{
	std::cout << std::endl << "on est dans CGI" << std::endl << std::endl;
//	req.print();
//	config.print();

	int pipefd[2];
	pipe(pipefd);

	pid_t pid = fork();

	if(pid == 0 ) // fils
	{
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);

		std::vector<std::string> envString;
		envString.push_back("REQUEST_METHOD=" + req.getMethod());
		envString.push_back("QUERY_STRING=" + req.getQueryString());
		envString.push_back("PATH_INFO=" + req.getPath());
		envString.push_back("SERVER_NAME=" + config.server_name);
		envString.push_back("SERVER_PORT=" + libftpp::str::StringUtils::itos(config.port));
		envString.push_back("CONTENT_TYPE=text/html"); //seulement POST
		//envString.push_back("SCRIPT_NAME="); //seulement POST // search.py
		//CONTENT_LENGTH //seulement si body(POST) //ok

		//SERVER_PROTOCOL //HTTP/1.1
		//SERVER_SOFTWARE //C++98?
		//GATEWAY_INTERFACE//CGI/2.4?

		//HTTP_*********** issus du header//??seb

		//REMOTE_ADDR //pas oblige
		//REMOTE_HOST //pas oblige
		//REMOTE_PORT //pas oblige
		//DOCUMENT_ROOT //pas oblige

		// https://datatracker.ietf.org/doc/html/rfc3875  chapt 4.1


		std::vector<char*>env;
		for(size_t i = 0; i < envString.size(); i++)
			env.push_back(const_cast<char*>(envString[i].c_str()));
		env.push_back(NULL);

//		std::cerr << "*** config.routes[i].root : " << config.routes[i].root << std::endl;

		char *root = const_cast<char*>(config.routes[i].root.c_str());

		char *args[] = {root, NULL};

		execve(root,args,env.data());
		perror("execve");
		_exit(1);
	}

	//parent
	close(pipefd[1]);

	std::string output;
	char buffer[1024];
	ssize_t n;

	while((n = read(pipefd[0], buffer, sizeof(buffer))) > 0)
		output.append(buffer,n);

	close(pipefd[0]);

	waitpid(pid, NULL, 0);

	return(output);
}

// std::string webserv::http::Get::execute(const webserv::http::Request& req, const ServerConfig& config, const RouteConfig& route)
// {
// 	int httpCode = 200;
// 	(void)httpCode; // FIXME : pas use pour l'instant mais comme la fontion n'est pas fini je le laisse

// 	std::string effectiveRoute = webserv::http::RouteMatcher::getEffectiveRoot(config, route);
// 	std::string fullPath = libftpp::str::PathUtils::join(effectiveRoute, req.getPath());

// 	if (fullPath.find(effectiveRoute) != 0)
// 		return ResponseBuilder::generateError(403, config);
// 	if (!libftpp::str::PathUtils::exists(fullPath))
// 		return ResponseBuilder::generateError(404, config);
// 		// index et auto index
// 	if (libftpp::str::PathUtils::isDirectory(fullPath)) {
// 		std::string indexName = config.index.empty() ? "index.html" : config.index;
// 		std::string indexPath = libftpp::str::PathUtils::join(fullPath, indexName);
// 		if (libftpp::str::PathUtils::exists(indexPath))
// 			fullPath = indexPath;
// 		else {
// 			if (route.directory_listing){
// 				//TODO SEB: Autoindex 
// 				std::cout << "===== AUTO INDEX =====" << std::endl;
// 				return "HTTP/1.1 200 OK\r\n\r\nAUTOINDEX TODO";
// 			}
// 			return ResponseBuilder::generateError(403, config);
// 		}
// 	}
// 	if (route.cgi == true) {
//         std::string ext = route.cgi_extension;
        
//         // Si le chemin finit par l'extension CGI (ex: .php)
//         if (fullPath.length() >= ext.length() && 
//             fullPath.compare(fullPath.length() - ext.length(), ext.length(), ext) == 0) 
//         {
// 			// for(size_t i = 0; i < config.routes.size(); i++) //SDU CGI, verifier ou il faut le positionner
// 	 		// {
// 	 			// if(req.getPath() == config.routes[i].path)
// 	 			// {
// 					// fullPath = "cgi.html";
// 					// 
// 					// effectiveRoute = execute_cgi(req, config, i);
// 					// return _createSuccessResponse(effectiveRoute, fullPath);
// 					// }
// 					// }
					
					
// 					std::cout << "===== ici = "<< req.getPath() << std::endl;
// 			//partie cgi
// 			// pas reussi a inclure la fonction actuel je pense elle devrait avoir besoin
// 			// de ces 4 info pour fonctionner ou en tout cas avec ces 4 la t'as tout en cas de besoin
//             //return execute_cgi(req, fullPath, config, route);
// 			return "HTTP/1.1 200 OK\r\n\r\n CG1§§§8I"; // <--- coucou de mon chat

//         }
//     }

// 	//static flie lire le fichier et le return au client
//     if (access(fullPath.c_str(), R_OK) != 0) {
//         return ResponseBuilder::generateError(403, config);
//     }
// 	std::cout << "===== static file =====" << std::endl;
//     std::string content = _readFile(fullPath);
//     struct stat s;
//     stat(fullPath.c_str(), &s);
//     if (content.empty() && s.st_size > 0) {
//         return ResponseBuilder::generateError(500, config);
//     }

//     return _createSuccessResponse(content, fullPath);
// }

std::string webserv::http::Get::execute(const http::Request& req, const ServerConfig& config, const RouteConfig& route)
{
	(void)route;
	int httpCode = 200;
	std::string content = "";
	std::string fullPath = ".html";

	if (httpCode != 200) 
		return ResponseBuilder::generateError(httpCode, config);

	//   http://37.59.120.163:9003/search?query=webserv   //SDU
	// TODO RAPH: modif getSecurePath pour aller avec cgi
	// remplir full path avec vrai path cgi et descendre le block en haut -> en bas
	fullPath = _getSecurePath(config.root, req.getPath(), httpCode);//SDU : trop restrictif
	//std::cerr << "fullPath = " <<  fullPath << " code = " << httpCode << std::endl;
	struct stat s;
	if (stat(fullPath.c_str(), &s) == 0 && (s.st_mode & S_IFDIR)) {
		if (!_checkIndexFile(fullPath, httpCode, config)) {
			// TODO: RAPH
			// Ici, plus tard : quelque cgose comme if (config.autoindex) return AutoIndex::gen(...);
			return ResponseBuilder::generateError(httpCode, config);
		}
	}
	
	if (access(fullPath.c_str(), R_OK) != 0)
	{
		return ResponseBuilder::generateError(403, config);
	}
		for(size_t i = 0; i < config.routes.size(); i++) //SDU CGI, verifier ou il faut le positionner
		{
			if(req.getPath() == config.routes[i].path)
			{
				std::cout << "===== ici = "<< req.getPath() << std::endl;
				fullPath = "cgi.html";
				
				content = execute_cgi(req, config, i);
				return _createSuccessResponse(content, fullPath);
			}
		}
	content = _readFile(fullPath);
	if (content.empty() && s.st_size > 0)
		return ResponseBuilder::generateError(500, config);
	
	return _createSuccessResponse(content, fullPath);
}
