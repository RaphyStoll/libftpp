#include"ConfigParser.hpp"

void DataConfig::ipp()
{
		_i++;
		if(_i >= _token.size())
			throw std::out_of_range("Acces token : " + _token[_i - 1]);
}

void DataConfig::dir_server()
{
	if(_token[_i] == "server_name")
	{
		ipp();
		_currentServer.server_name = _token[_i];
		_i++;
	}
	else if(_token[_i] == "listen")
	{
		ipp();
		if (valid_ipv4() == -1)
			throw std::runtime_error("ipv4 adress ko : " + _token[_i]);
		_currentServer.listen = _token[_i];
		_i++;
	}
	else if(_token[_i] == "port")
	{
		ipp();
		valid_port();
		_currentServer.port = libftpp::str::StringUtils::stoi(_token[_i]);
		_i++;
	}
	else if(_token[_i] == "root")
	{
		ipp();
		valid_path();
		_currentServer.root = _token[_i];
		_i++;
	}
	else if(_token[_i] == "autoindex")
	{
		ipp();
		if(_token[_i] == "off")
			_currentServer.autoindex = 0;
		else if(_token[_i] == "on")
			_currentServer.autoindex = 1;
		else
			std::runtime_error("Unknow directory_listing directive : " + _token[_i]);
		_i++;
	}
	else if(_token[_i] == "index")
	{
		ipp();
		valid_file();
		_currentServer.index = _token[_i];
		_i++;
	}
	else if(_token[_i] == "max_body_size")
	{
		ipp();
		valid_max_body_size();
		_currentServer.max_body_size = libftpp::str::StringUtils::stoi(_token[_i]);
		_i++;
	}
	else if(_token[_i] == "error_page")
	{
		_i++;
		if(_i + 1 >= _token.size())
			throw std::out_of_range("Acces token : " + _token[_i - 1]);
		size_t n = libftpp::str::StringUtils::stoi(_token[_i]);
		valid_error_code();
		_i++;
		valid_path();
		_currentServer.error_pages.insert(std::make_pair(n,_token[_i]));
		_i++;
	}
	else
	{
		throw std::runtime_error("Unknow server directive : " +  _token[_i] );
		_i++;
	}
}

void DataConfig::dir_route()
{
	if(_token[_i] == "root")
	{
		ipp();
		valid_path();
		_currentRoute.root = _token[_i];
		_i++;
	}
	else if(_token[_i] == "methods")
	{
		ipp();
		while(_token[_i] == "GET" || _token[_i] == "POST" || _token[_i] == "DELETE")
		{
			_currentRoute.methods.push_back(_token[_i]);
			_i++;
		}
	}
	else if(_token[_i] == "directory_listing")
	{
		ipp();
		if(_token[_i] == "off")
			_currentRoute.directory_listing = 0;
		else if(_token[_i] == "on")
			_currentRoute.directory_listing = 1;
		else
			std::runtime_error("Unknow directory_listing directive : " + _token[_i]);
		_i++;
	}
	else if(_token[_i] == "upload")
	{
		ipp();
		if(_token[_i] == "off")
			_currentRoute.upload = 0;
		else if(_token[_i] == "on")
			_currentRoute.upload = 1;
		else
			std::runtime_error("Unknow upload directive : " + _token[_i]);
		_i++;
	}
	else if(_token[_i] == "upload_path")
	{
		ipp();
		valid_path();
		_currentRoute.upload_path = _token[_i];
		_i++;
	}
	else if(_token[_i] == "cgi")
	{
		ipp();
		if(_token[_i] == "off")
			_currentRoute.cgi = 0;
		else if(_token[_i] == "on")
			_currentRoute.cgi = 1;
		else
			std::runtime_error("Unknow upload directive : " + _token[_i]);
		_i++;
	}
	else if(_token[_i] == "cgi_extension")
	{
		ipp();
		_currentRoute.cgi_extension = _token[_i];
		_i++;
	}
	else if(_token[_i] == "cgi_path")
	{
		ipp();
//		valid_path(); //SDU chiant pour les test, a remettre.
		_currentRoute.cgi_path = _token[_i];
		_i++;
	}
	else if(_token[_i] == "redirect")
	{
		_i++;
		if(_i + 1 >= _token.size())
			throw std::out_of_range("Acces token : " + _token[_i - 1]);
		size_t n = libftpp::str::StringUtils::stoi(_token[_i]);
		_currentRoute.redirect.insert(std::make_pair(n,_token[_i + 1]));
		_i += 2;
		//completer les verif de doublons et compagnie si besoin
	}
	else
		throw std::runtime_error("Unknow route directive : " +  _token[_i]);
}