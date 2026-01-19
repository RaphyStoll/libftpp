#include"ConfigParser.hpp"

ServerConfig::ServerConfig() :
	listen(""),
	port(-1),
	root(""),
	index(""),
	max_body_size(0),
	error_pages(),
	routes()
{
}
