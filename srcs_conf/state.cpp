#include"ConfigParser.hpp"

void state_global(DataConfig *data)
{
	std::vector<std::string> token = data->token;
	size_t i = data->i;

	if(i + 1 < token.size() && token[i] == "server" && token[i + 1] == "{")
	{
		data->state = IN_SERVER;
		data->currentServer = ServerConfig();
		i += 2;
//		std::cout << "in server" << std::endl;
	}
	else
	{
		std::cerr << "invalid directiv" << std::endl;
		i++;
	}
	data->i = i;
}

void state_server(DataConfig *data)
{
	std::vector<std::string> token = data->token;
	size_t i = data->i;

	if(token[i] == "}")
	{
		data->state = GLOBAL;
		data->servers.push_back(data->currentServer);
		i++;
//		std::cout << "in global" << std::endl;
		data->i = i;
	}
	else if(i + 2 < token.size() && token[i] == "route" && token[i + 2] == "{")
	{
		data->state = IN_ROUTE;
		data->currentRoute = RouteConfig();
		data->currentRoute.path = token[i+1];
		i += 3;
//		std::cout << "in route" << std::endl;
		data->i = i;
	}
	else
	{	
//		std::cout << i << " " << std::endl;
		dir_server(data);
	}

}

void state_route(DataConfig *data)
{
	std::vector<std::string> token = data->token;
	size_t i = data->i;

	if(token[i] == "}")
	{
		data->state = IN_SERVER;
		data->currentServer.routes.push_back(data->currentRoute);
		i++;
//		std::cout << "de retour in server" << std::endl;
		data->i = i;
	}
	else
	{
		dir_route(data);
//		std::cout << i << " " << std::endl;
//		i++;
	}

}