#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <cctype>

#include"StringUtils.hpp"

struct RouteConfig
{
	std::string path;
	std::string root;
	std::vector<std::string> methods;
	bool directory_listing;
	bool upload;
	std::string upload_path;
	bool cgi;
	std::string cgi_extention;
	std::string cgi_path;

	RouteConfig();
};

struct ServerConfig
{
	std::string listen;
	int port;
	std::string root;
	std::string index;
	size_t max_body_size;
	std::map<int, std::string> error_pages;
	std::vector<RouteConfig> routes;

	ServerConfig();
};

struct DataConfig {
	std::vector<std::string> brut_line;
	std::vector<std::string> token;
	std::vector<ServerConfig> servers;
};

enum ParseState {
	GLOBAL,
	IN_SERVER,
	IN_ROUTE
};

void init_server (ServerConfig &server);

#endif