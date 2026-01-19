#include"ConfigParser.hpp"

void dir_server(DataConfig *data)
{
	

	std::vector<std::string> token = data->token;
	size_t i = data->i;

//	std::cerr << " in dir server " ;

	if(token[i] == "listen")
	{
		i++;
		if(i >= token.size())
			std::cerr << "erreur acces token" << std::endl; //SDU il faut sortir sinon segfault
		data->currentServer.listen = token[i];
		i++;
	}
	else if(token[i] == "port")
	{
		i++;
		if(i >= token.size())
			std::cerr << "erreur acces token" << std::endl; //SDU il faut sortir sinon segfault
		data->currentServer.port = libftpp::str::StringUtils::stoi(token[i]); //SDU :trop long
		i++;
	}
	else if(token[i] == "root")
	{
		i++;
		if(i >= token.size())
			std::cerr << "erreur acces token" << std::endl; //SDU il faut sortir sinon segfault
		data->currentServer.root = token[i];
		i++;
	}
	else if(token[i] == "index")
	{
		i++;
		if(i >= token.size())
			std::cerr << "erreur acces token" << std::endl; //SDU il faut sortir sinon segfault
		data->currentServer.index = token[i];
		i++;
	}
	else if(token[i] == "max_body_size")
	{
		i++;
		if(i >= token.size())
			std::cerr << "erreur acces token" << std::endl; //SDU il faut sortir sinon segfault
		data->currentServer.max_body_size = libftpp::str::StringUtils::stoi(token[i]); // SDU trop long
		i++;
	}
	else if(token[i] == "error_page")
	{
		i++;
		if(i + 1 >= token.size())
			std::cerr << "erreur acces token" << std::endl; //SDU il faut sortir sinon segfault
		size_t n = libftpp::str::StringUtils::stoi(token[i]);
		data->currentServer.error_pages.insert(std::make_pair(n,token[i + 1])); // SDU trop long
		i++;
	}
	else
	{
		std::cerr << "Unknow directive : " << token[i] << " in server" << std::endl;
		i++;
	}
	data->i = i;
}

/*
	std::string path;
	std::string root;
	std::vector<std::string> methods;
	bool directory_listing;
	bool upload;
	std::string upload_path;
	bool cgi;
	std::string cgi_extention;
	std::string cgi_path;
*/

void dir_route(DataConfig *data)
{
	

	std::vector<std::string> token = data->token;
	size_t i = data->i;

//	std::cerr << " in dir route " ;

	if(token[i] == "path")
	{
		i++;
		if(i >= token.size())
			std::cerr << "erreur acces token" << std::endl; //SDU il faut sortir sinon segfault
		data->currentRoute.path = token[i];
		i++;
	}
	else if(token[i] == "root")
	{
		i++;
		if(i >= token.size())
			std::cerr << "erreur acces token" << std::endl; //SDU il faut sortir sinon segfault
		data->currentRoute.root = token[i];
		i++;
	}
	else if(token[i] == "methods")
	{
		i++;
		if(i >= token.size())
			std::cerr << "erreur acces token" << std::endl; //SDU il faut sortir sinon segfault
		while(token[i] == "GET" || token[i] == "POST" || token[i] == "DELETE")
		{
			data->currentRoute.methods.push_back(token[i]);
			i++;
		}
	}
	else if(token[i] == "directory_listing")
	{
		i++;
		if(i >= token.size())
			std::cerr << "erreur acces token" << std::endl; //SDU il faut sortir sinon segfault
		if(token[i] == "off")
			data->currentRoute.directory_listing = 0;
		else if(token[i] == "on")
			data->currentRoute.directory_listing = 1;
		else
			std::cerr << "Unknow directive : " << token[i] << " in route" << std::endl;
		i++;
	}
	else
	{
		std::cerr << "Unknow directive : " << token[i] << " in route" << std::endl;
		i++;
	}
	
	data->i = i;
}