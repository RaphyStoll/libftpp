#include "Get.hpp"
#include "ResponseBuilder.hpp"
#include "libftpp.hpp"

#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/stat.h>
#include <climits>
#include <cstdlib>
#include <sys/wait.h>



std::string execute_cgi(const ::http::Request& req, const ServerConfig& config, size_t i)
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


std::string webserv::http::Get::execute(const ::http::Request& req, const ServerConfig& config)
{
	int httpCode = 200;
	std::string content = "";
	std::string fullPath = "";


	if (httpCode != 200) 
		return ResponseBuilder::generateError(httpCode, config);

	//   http://37.59.120.163:9003/search?query=webserv   //SDU
	for(size_t i = 0; i < config.routes.size(); i++) //SDU CGI, verifier ou il faut le positionner
	{
		if(req.getPath() == config.routes[i].path)
		{
			content = execute_cgi(req, config, i);
			return _createSuccessResponse(content, fullPath);
		}
	}

	fullPath = _getSecurePath(config.root, req.getPath(), httpCode);//SDU : trop restrictif
	std::cerr << "fullPath = " <<  fullPath << " code = " << httpCode << std::endl;
	struct stat s;
	if (stat(fullPath.c_str(), &s) == 0 && (s.st_mode & S_IFDIR)) {
		if (!_checkIndexFile(fullPath, httpCode)) {
			// TODO: RAPH
			// Ici, plus tard : quelque cgose comme if (config.autoindex) return AutoIndex::gen(...);
			return ResponseBuilder::generateError(httpCode, config);
		}
	}
	
	if (access(fullPath.c_str(), R_OK) != 0)
	{
		return ResponseBuilder::generateError(403, config);
	}
	content = _readFile(fullPath);
	if (content.empty() && s.st_size > 0)
		return ResponseBuilder::generateError(500, config);
	std::cerr << "fullPath = " <<  fullPath << " code = " << httpCode << std::endl;
	

	return _createSuccessResponse(content, fullPath);
}
