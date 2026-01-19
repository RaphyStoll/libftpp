# include "ConfigParser.hpp"

void print_vect(const std::vector<std::string> &vect)
{
	size_t i = 0;
	while(i < vect.size())
	{
		std::cout << "vect[" << i << "] : " << vect[i] << std::endl;
		i++;
	}
}

void print_route (const RouteConfig &route)
{
	std::cout << "  path : " << route.path << std::endl;
	std::cout << "  root : " << route.root << std::endl;

	std::cout << "  methods : ";
	for(size_t i = 0 ; i < route.methods.size(); i++)
		std::cout << route.methods[i] << " ";
	std::cout << std::endl;

	std::cout << "  directory_listing : " << route.directory_listing << std::endl;
	

}

void print_server (const ServerConfig &serv)
{
	std::cout << " listen : " << serv.listen << std::endl;
	std::cout << " port : " << serv.port << std::endl;
	std::cout << " root : " << serv.root << std::endl;
	std::cout << " index : " << serv.index << std::endl;
	std::cout << " max_body_size : " << serv.max_body_size << std::endl;

	for (std::map<int, std::string>::const_iterator it = serv.error_pages.begin(); it != serv.error_pages.end(); ++it)
	{
		std::cout << " error_page " << it->first << " : " << it->second << std::endl;
	}

	
	for(size_t i = 0; i < serv.routes.size(); i++)
	{
		std::cout << " Route " << i << " :"<<std::endl;
		print_route(serv.routes[i]);
	}
	
	

}



void print_conf (const std::vector<ServerConfig> &serv)
{
	size_t i;

	std::cout << "****** Conf ******" << std::endl;
	std::cout << std::endl;

	i = 0;
	while(i < serv.size())
	{
		std::cout << "Server " << i << " :" << std::endl;
		print_server(serv[i]);
		i++;
	}

}