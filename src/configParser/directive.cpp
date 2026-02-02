#include"ConfigParser.hpp"

void dir_server(DataConfig *data)
{
	std::vector<std::string> token = data->token;
	size_t i = data->i;

	if(token[i] == "server_name")
	{
		i++;
		if(i >= token.size())
			throw std::out_of_range("Acces token : " + token[i-1]);
		data->currentServer.server_name = token[i];
		i++;
	}
	else if(token[i] == "listen")
	{
		i++;
		if(i >= token.size())
			throw std::out_of_range("Acces token : " + token[i-1]);
		data->currentToken = token[i];
		if (valid_ipv4(data) == -1)
			throw std::runtime_error("ipv4 adress ko : " + data->currentToken);
			//std::cout << token[i] << " KO" << std::endl;
		data->currentServer.listen = token[i];
		i++;
	}
	else if(token[i] == "port")
	{
		i++;
		if(i >= token.size())
			throw std::out_of_range("Acces token : " + token[i-1]);

		data->currentToken = token[i];
		valid_port(data);
		data->currentServer.port = libftpp::str::StringUtils::stoi(token[i]);
		i++;
	}
	else if(token[i] == "root")
	{
		i++;
		if(i >= token.size())
			throw std::out_of_range("Acces token : " + token[i-1]);
		data->currentToken = token[i];
		valid_path(data);
		data->currentServer.root = token[i];
		i++;
	}
	else if(token[i] == "autoindex")
	{
		i++;
		if(i >= token.size())
			throw std::out_of_range("Acces token : " + token[i-1]);
		if(token[i] == "off")
			data->currentServer.autoindex = 0;
		else if(token[i] == "on")
			data->currentServer.autoindex = 1;
		else
			std::runtime_error("Unknow directory_listing directive : " + token[i]);
		i++;
	}
	else if(token[i] == "index")
	{
		i++;
		if(i >= token.size())
			throw std::out_of_range("Acces token : " + token[i-1]);
		data->currentToken = token[i];
		valid_path(data);
		data->currentServer.index = token[i];
		i++;
	}
	else if(token[i] == "max_body_size")
	{
		i++;
		if(i >= token.size())
			throw std::out_of_range("Acces token : " + token[i-1]);
		data->currentToken = token[i];
		valid_max_body_size(data);
		data->currentServer.max_body_size = libftpp::str::StringUtils::stoi(token[i]);
		i++;
	}
	else if(token[i] == "error_page")
	{
		i++;
		if(i + 1 >= token.size())
			throw std::out_of_range("Acces token : " + token[i-1]);
		size_t n = libftpp::str::StringUtils::stoi(token[i]);
		data->currentToken = token[i];
		valid_error_code(data);
		data->currentToken = token[i + 1];
		valid_path(data);
		data->currentServer.error_pages.insert(std::make_pair(n,token[i + 1]));
		i+=2;
	}
	else
	{
		throw std::runtime_error("Unknow server directive : " +  token[i] );
		i++;
	}
	data->i = i;
}

void dir_route(DataConfig *data)
{
	

	std::vector<std::string> token = data->token;
	size_t i = data->i;

	if(token[i] == "root")
	{
		i++;
		if(i >= token.size())
			throw std::out_of_range("Acces token : " + token[i-1]);
		data->currentToken = token[i];
		valid_path(data);
		data->currentRoute.root = token[i];
		i++;
	}
	else if(token[i] == "methods")
	{
		i++;
		if(i >= token.size())
			throw std::out_of_range("Acces token : " + token[i-1]);
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
			throw std::out_of_range("Acces token : " + token[i-1]);
		if(token[i] == "off")
			data->currentRoute.directory_listing = 0;
		else if(token[i] == "on")
			data->currentRoute.directory_listing = 1;
		else
			std::runtime_error("Unknow directory_listing directive : " + token[i]);
		i++;
	}
	else if(token[i] == "upload")
	{
		i++;
		if(i >= token.size())
			throw std::out_of_range("Acces token : " + token[i-1]);
		if(token[i] == "off")
			data->currentRoute.upload = 0;
		else if(token[i] == "on")
			data->currentRoute.upload = 1;
		else
			std::runtime_error("Unknow upload directive : " + token[i]);
		i++;
	}
	else if(token[i] == "upload_path")
	{
		i++;
		if(i >= token.size())
			throw std::out_of_range("Acces token : " + token[i-1]);
		data->currentToken = token[i];
		valid_path(data);
		data->currentRoute.upload_path = token[i];
		i++;
	}
	else if(token[i] == "cgi")
	{
		i++;
		if(i >= token.size())
			throw std::out_of_range("Acces token : " + token[i-1]);
		if(token[i] == "off")
			data->currentRoute.cgi = 0;
		else if(token[i] == "on")
			data->currentRoute.cgi = 1;
		else
			std::runtime_error("Unknow upload directive : " + token[i]);
		i++;
	}
	else if(token[i] == "cgi_extension")
	{
		i++;
		if(i >= token.size())
			throw std::out_of_range("Acces token : " + token[i-1]);
		data->currentRoute.cgi_extension = token[i];
		i++;
	}
	else if(token[i] == "cgi_path")
	{
		i++;
		if(i >= token.size())
			throw std::out_of_range("Acces token : " + token[i-1]);
		data->currentToken = token[i];
//		valid_path(data); //SDU chiant pour les test, a remettre.
		data->currentRoute.cgi_path = token[i];
		i++;
	}
	else if(token[i] == "redirect")
	{
		i++;
		if(i + 1 >= token.size())
			throw std::out_of_range("Acces token : " + token[i-1]);
		size_t n = libftpp::str::StringUtils::stoi(token[i]);
		data->currentRoute.redirect.insert(std::make_pair(n,token[i + 1]));
		i+=2;
		//completer les verif de doublons et compagnie si besoin
	}
	else
		throw std::runtime_error("Unknow route directive : " +  token[i]);

	data->i = i;
}