#include"ConfigParser.hpp"
#include<sys/stat.h>
#include<unistd.h>
#include<cstdlib>

static bool is_digit(char c) //SDU mettre ailleur?
{
	return (c >= '0' && c <= '9');
}

void valid_port(DataConfig *data)
{
	int var = libftpp::str::StringUtils::stoi(data->currentToken);
	if (var < PORT_MIN || var > PORT_MAX)
		throw std::runtime_error("port out of range : " + data->currentToken);
//	for(size_t i = 0; i < data->servers.size(); i++)
//	{
//		if(data->servers[i].port == var)
//			throw std::runtime_error("port already in use in config : " + data->currentToken);
//	}
}

void valid_max_body_size(DataConfig *data)
{
	size_t var = libftpp::str::StringUtils::stoi(data->currentToken);
	if (var <= MIN_BODY_SIZE_LIMIT || var > MAX_BODY_SIZE_LIMIT)
		throw std::runtime_error("max_body_size out of range : " + data->currentToken);
}

void valid_error_code(DataConfig *data)
{
	size_t var = libftpp::str::StringUtils::stoi(data->currentToken);
	if (var < ERROR_CODE_MIN || var > ERROR_CODE_MAX)
		throw std::runtime_error("error code out of range : " + data->currentToken);
	if(data->currentServer.error_pages.count(var) > 0)
			throw std::runtime_error("error code already used in config : " + data->currentToken);
}

void valid_path(DataConfig *data)
{
	struct stat st;
	if (stat(data->currentToken.c_str(), &st) != 0)
		throw std::runtime_error("path ko : " + data->currentToken);
	if (access(data->currentToken.c_str(), R_OK) != 0)
		throw std::runtime_error("path access ko : " + data->currentToken);
}

int valid_ipv4(DataConfig *data)
{
	std::string ip = data->currentToken;

	size_t i = 0;
	size_t j = 0;
	std::string s;

	while(j < 4 && i < ip.size())
	{
		s = "";
		if(!is_digit(ip[i]))
			return(-1);
		while(is_digit(ip[i]))
			s += ip[i++];
		if(std::atoi(s.c_str()) < 0 || std::atoi(s.c_str()) > 255)
			return(-1);
		if(j < 2 && ip[i] != '.')
			return(-1);
		i++;
		j++;
	}
	if(i != ip.size() + 1 || j != 4)
		return(-1);
	return(0);
}