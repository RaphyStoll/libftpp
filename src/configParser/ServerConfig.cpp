#include"ConfigParser.hpp"

ServerConfig::ServerConfig() :
	listen(""),
	port(-1),
	root(""),
	index(""),
	max_body_size(0),
	error_pages(),
	routes(),
	autoindex(0)
{
}

void ServerConfig::print(void) const
{
	std::cout << " server_name : " << server_name << std::endl;
	std::cout << " listen : " << listen << std::endl;
	std::cout << " port : " << port << std::endl;
	std::cout << " root : " << root << std::endl;
	if(autoindex  == true)
		std::cout << " autoindex : on" << std::endl;
	else
		std::cout << " autoindex : off" << std::endl;
	std::cout << " index : " << index << std::endl;
	std::cout << " max_body_size : " << max_body_size << std::endl;

	for (std::map<int, std::string>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it)
	{
		std::cout << " error_page " << it->first << " : " << it->second << std::endl;
	}

	
	for(size_t i = 0; i < routes.size(); i++)
	{
		std::cout << " Route " << i << " :"<<std::endl;
		RouteConfig r0 = routes[i];
		
		routes[i].print();
	}
}