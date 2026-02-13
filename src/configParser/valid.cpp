#include"ConfigParser.hpp"


static bool is_digit(char c) //SDU mettre ailleur?
{
	return (c >= '0' && c <= '9');
}

void DataConfig::valid_port()
{
	int var = libftpp::str::StringUtils::stoi(_token[_i]);
	if (var < PORT_MIN || var > PORT_MAX)
		throw std::runtime_error("port out of range : " + _token[_i]);
//	for(size_t i = 0; i < _servers.size(); i++)
//	{
//		if(_servers[i].port == var)
//			throw std::runtime_error("port already in use in config : " + _token[_i]);
//	}
}

void DataConfig::valid_max_body_size()
{
	size_t var = libftpp::str::StringUtils::stoi(_token[_i]);
	if (var <= MIN_BODY_SIZE_LIMIT || var > MAX_BODY_SIZE_LIMIT)
		throw std::runtime_error("max_body_size out of range : " + _token[_i]);
}

void DataConfig::valid_error_code()
{
	size_t var = libftpp::str::StringUtils::stoi(_token[_i]);
	if (var < ERROR_CODE_MIN || var > ERROR_CODE_MAX)
		throw std::runtime_error("error code out of range : " + _token[_i]);
	if(_currentServer.error_pages.count(var) > 0)
			throw std::runtime_error("error code already used in config : " + _token[_i]);
}

void DataConfig::valid_path()
{
	struct stat st;
	if (stat(_token[_i].c_str(), &st) != 0)
		throw std::runtime_error("path ko : " + _token[_i]);
	if (access(_token[_i].c_str(), R_OK) != 0)
		throw std::runtime_error("path access ko : " + _token[_i]);
}

void DataConfig::valid_file()
{
	std::string s = _token[_i];
	if(s.empty())
		throw std::runtime_error("no file : " + s);
	if(s.find('/') != std::string::npos || s.find("\\") != std::string::npos || s.find("..") != std::string::npos)
		throw std::runtime_error("forbiden characters : " + s);
	if(s == "." || s == "..")
		throw std::runtime_error("forbiden characters : " + s);
	for(size_t i = 0; i < s.size(); i++)
	{
		unsigned char c = s[i];
		if (std::iscntrl(c))
			throw std::runtime_error("forbiden characters : " + s);
	}
	s = _currentServer.root + "/" + s;

	//std::cerr << "s = " << s << std::endl;
	struct stat st;
	if (stat(s.c_str(), &st) != 0)
		throw std::runtime_error("file ko : " + _token[_i]);
	if (access(s.c_str(), R_OK) != 0)
		throw std::runtime_error("file access ko : " + _token[_i]);
}

int DataConfig::valid_ipv4()
{
	std::string ip = _token[_i];

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