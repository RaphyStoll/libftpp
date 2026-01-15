# include "ConfigParser.hpp"

static bool supprComment(std::string &line)
{
	size_t commentPos = line.find('#');

	if(commentPos != std::string::npos)
		line = line.substr(0, commentPos);

	line = libftpp::str::StringUtils::trim(line);

	return(0);
	
}

void print_vect(const std::vector<std::string> &vect)
{
	size_t i = 0;
	while(i < vect.size())
	{
		std::cout << "vect[" << i << "] : " << vect[i] << std::endl;
		i++;
	}
}

void tockenize(DataConfig *data)
{
	std::string line;
	std::string word;
	char c;

	size_t i;
	size_t j;
	
	i = 0;
	while(i < data->brut_line.size())
	{
		word.clear();
		line = data->brut_line[i];
		j = 0;
		while(j < line.size())
		{
			c = line[j];
			if(c == ' ')
			{
				if(! word.empty())
					data->token.push_back(word);
				word.clear();
			}
			else if (c == '{' || c == '}')
			{
				if(! word.empty())
					data->token.push_back(word);
				word.clear();
				word += c;
				if(! word.empty())
					data->token.push_back(word);
				word.clear();
			}
			else
			{
				word += c;
			}
			j++;
		}
		if(! word.empty())
			data->token.push_back(word);
		i++;
	}
}

void pars(DataConfig *data)
{
	ParseState state = GLOBAL;
	ServerConfig currentServer;
	RouteConfig currentRoute;

	std::vector<std::string> token = data-> token;
	size_t i = 0;

	(void) data;
	std::cout << "token.size = " << token.size() << std::endl;

	while(i < token.size())
	{
		if(state == GLOBAL)
		{
			if(token[i] == "server" && token[i + 1] == "{")
			{
				state = IN_SERVER;
				currentServer= ServerConfig();
				i += 2;
				std::cout << "in server" << std::endl;
			}
			else
			{
				std::cerr << "invalid directiv" << std::endl;
				i++;
			}
		}
		else if(state == IN_SERVER)
		{
			if(token[i] == "}")
			{
				state = GLOBAL;
				data->servers.push_back(currentServer);
				i++;
				std::cout << "in global" << std::endl;
			}
			else if(token[i] == "route" && token[i + 1] == "/php")
			{
				state = IN_ROUTE;
				currentRoute = RouteConfig();
				i += 2;
				std::cout << "in route" << std::endl;
			}
			else
			{
				std::cout << "autre" << std::endl;
				i++;
			}
		}
		if(state == IN_ROUTE)
		{
			if(token[i] == "}")
			{
				state = IN_SERVER;
				currentServer.routes.push_back(currentRoute);
				i++;
				std::cout << "de retour in server" << std::endl;
			}
			else
			{
				std::cout << "autre" << std::endl;
				i++;
			}
		}
	}

	std::cout << "fin du parsing" << std::endl;
}

static bool openFileAndParseConfig(const std::string & config_path, DataConfig * data)
{
	std::cout << "config_path = " << config_path << std::endl;

	std::ifstream file(config_path);
	if(!file)
	{
		std::cerr << "fichier introuvable" << std::endl;
		return (-1);
	}

	std::string line;
	while( std::getline(file, line))
	{
		supprComment(line);
		if(!line.empty())
			data->brut_line.push_back(line);
	}

	if(data->brut_line.empty()) //tester + tard??
	{
		std::cerr << "fichier de config vide" << std::endl;
		return (-1);
	}

	print_vect(data->brut_line);
	std::cout << std::endl;
	tockenize(data);
	print_vect(data->token);
	std::cout << std::endl;
	pars(data);


	return(1);
}

int main () 
{
	std::cout << "lancement de webserv" << std::endl;
	DataConfig data;

	std::string config_folder = "config";
	std::string config_file = "config.conf";
	std::string config_path = config_folder + "/" + config_file;

	if (!openFileAndParseConfig(config_path, &data))
	{
		std::cerr << "error openFileAndParseConfig" << std::endl;
		return (-1);
	}

	return(0);
}