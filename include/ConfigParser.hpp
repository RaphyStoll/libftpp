#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <cctype>
#include <stdexcept>

//#include "Utils.hpp"

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

	public :

	void print(void) const;

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
	bool autoindex; //on

	ServerConfig();

	void print(void) const;
};

class DataConfig {

	public :

	DataConfig();
	~DataConfig();
	
	std::string config_path;
	std::vector<ServerConfig> servers;
	
	void	cParser();


	private :

	size_t _i;
	ParseState _state;
	std::vector<std::string> _brutLine;
	std::vector<std::string> _token;
	ServerConfig _currentServer;
	RouteConfig _currentRoute;


	void 	openConf();
	void	tockenize();
	void	pars_state();

	void	state_global();
	void	state_server();
	void	state_route();

	void	dir_server();
	void	dir_route();

	void	ipp();
	void	valid_port();
	void	valid_max_body_size();
	void	valid_error_code();
	void	valid_path();
	void	valid_file();
	int		valid_ipv4();

};

#endif