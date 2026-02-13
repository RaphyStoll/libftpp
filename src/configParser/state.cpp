#include"ConfigParser.hpp"

void DataConfig::state_global()
{
	if(_i + 1 < _token.size() && _token[_i] == "server" && _token[_i + 1] == "{")
	{
	//	std::cout << "in server 1" << std::endl;
		_state = IN_SERVER;
		_currentServer = ServerConfig();
		_i += 2;
	}
	else
		throw std::runtime_error("Invalid global directive : " + _token[_i]);
}

void DataConfig::state_server()
{
	if(_token[_i] == "}")
	{
		_state = GLOBAL;
		servers.push_back(_currentServer);
		_i++;
//		std::cout << "in global" << std::endl;
	}
	else if(_i + 2 < _token.size() && _token[_i] == "route" && _token[_i + 2] == "{")
	{
		_state = IN_ROUTE;
		_currentRoute = RouteConfig();
//		valid_path(); //SDU a remettre, mais trop chiant pour les test
		_currentRoute.path = _token[_i + 1];
		_i += 3;
//		std::cout << "in route" << std::endl;
	}
	else
		dir_server();
}

void DataConfig::state_route()
{
	if(_token[_i] == "}")
	{
//		std::cout << "in server 2" << std::endl;
		_state = IN_SERVER;
		_currentServer.routes.push_back(_currentRoute);
		_i++;
	}
	else if(_token[_i] == "{")
		throw std::runtime_error("too much block {}");
	else
		dir_route();
}

