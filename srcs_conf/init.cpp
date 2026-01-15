#include"ConfigParser.hpp"

ServerConfig::ServerConfig() :
	listen(""),
	port(80),
	root(""),
	index(""),
	max_body_size(128),
	error_pages(),
	routes()
{
}

RouteConfig::RouteConfig() :
	path(""),
	root(""),
	methods(),
	directory_listing(0),
	upload(),
	upload_path(),
	cgi(),
	cgi_extention(""),
	cgi_path("")
{
}