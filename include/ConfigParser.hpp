#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <cctype>
#include <stdexcept>

# include "../lib/LIBFTPP/include/libftpp.hpp"

#define MAX_BODY_SIZE_LIMIT (1024 * 1024)
#define MIN_BODY_SIZE_LIMIT 0
#define PORT_MIN 1024
#define PORT_MAX 65535
#define ERROR_CODE_MIN 400 //100-400 pour les code non error
#define ERROR_CODE_MAX 599



enum ParseState
{
	GLOBAL,
	IN_SERVER,
	IN_ROUTE
};

class RouteConfig
{
	public :

	std::string path; // route /upload
	std::string root; // root ./www/site1
	std::vector<std::string> methods; // [GET ,POST]
	bool directory_listing; // on
	bool upload; // on
	std::string upload_path; // ./www/site1/uploads
	bool cgi; //on
	std::string cgi_extension; // .php
	std::string cgi_path; // /usr/bin/php-cgi
	std::map<int, std::string> redirect; //redirect 301 /new

	RouteConfig();
};

class ServerConfig
{
	public :

	std::string server_name;
	std::string listen; // 127.0.0.1
	int port; // 8080
	std::string root; // ./www/site1
	std::string index; // index.html
	size_t max_body_size; // 1000000
	std::map<int, std::string> error_pages; // error_page 404 ./errors/404.html
	std::vector<RouteConfig> routes; // route/..

	ServerConfig();
};

struct DataConfig {
	std::string config_path;
	std::vector<std::string> brut_line;
	std::vector<std::string> token;
	std::vector<ServerConfig> servers;
	ParseState state;
	ServerConfig currentServer;
	RouteConfig currentRoute;
	std::string currentToken;
	size_t i;
};

void openFileAndParseConfig(DataConfig *data);

void state_global(DataConfig *data);
void state_server(DataConfig *data);
void state_route(DataConfig *data);

void dir_server(DataConfig *data);
void dir_route(DataConfig *data);

void print_vect(const std::vector<std::string> &vect);
void print_server (const ServerConfig &serv);
void print_route (const RouteConfig &route);
void print_conf (const std::vector<ServerConfig> &serv);

void valid_port(DataConfig *data);
void valid_max_body_size(DataConfig *data);
void valid_error_code(DataConfig *data);
void valid_path(DataConfig *data);
int valid_ipv4(DataConfig *data);


#endif