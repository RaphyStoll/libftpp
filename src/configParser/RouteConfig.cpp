#include"ConfigParser.hpp"

RouteConfig::RouteConfig() :
	path(""),
	root(""),
	methods(),
	directory_listing(false),
	upload(false),
	upload_path(""),
	cgi(false),
	cgi_extension(""),
	cgi_path("")
{
}

void RouteConfig::print(void) const
{
	std::cout << "  path : " << path << std::endl;
	std::cout << "  root : " << root << std::endl;

	std::cout << "  methods : ";
	for(size_t i = 0 ; i < methods.size(); i++)
		std::cout << methods[i] << " ";
	std::cout << std::endl;

	std::cout << "  directory_listing : " << directory_listing << std::endl;

}