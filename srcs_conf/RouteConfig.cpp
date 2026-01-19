#include"ConfigParser.hpp"

RouteConfig::RouteConfig() :
	path(""),
	root(""),
	methods(),
	directory_listing(false),
	upload(false),
	upload_path(""),
	cgi(false),
	cgi_extention(""),
	cgi_path("")
{
}