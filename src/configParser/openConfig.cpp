# include "ConfigParser.hpp"
# include "../../lib/LIBFTPP/include/libftpp.hpp"

static void suppr_comment(std::string &line)
{
	size_t commentPos = line.find('#');

	if(commentPos != std::string::npos)
		line = line.substr(0, commentPos);

	line = libftpp::str::StringUtils::trim(line);
}

static void tockenize(DataConfig *data)
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

static void pars_state(DataConfig *data)
{
	data->i = 0;
	data->state = GLOBAL;
	std::vector<std::string> token = data-> token;
//	std::cout << "token.size = " << token.size() << std::endl;

	while(data->i < token.size())
	{
		if(data->state == GLOBAL)
		{
			state_global(data);
		}
		else if(data->state == IN_SERVER)
		{
			state_server(data);
		}
		if(data->state == IN_ROUTE)
		{
			state_route(data);
		}
	}

	if (data->state != GLOBAL)
		throw std::runtime_error("block {} unclosed");

}

static void open_conf(DataConfig *data)
{
	std::ifstream file(data->config_path.c_str(), std::ios::in);
	if (!file.is_open())
	{
		throw std::runtime_error("\"" + data->config_path + "\" introuvable");
	}

	std::string line;
	while( std::getline(file, line))
	{
		suppr_comment(line);
		if(!line.empty())
			data->brut_line.push_back(line);
	}

	if(data->brut_line.empty()) //SDU inutil, tester + tard??
	{
		throw std::runtime_error("fichier de config vide");
	}
}

void openFileAndParseConfig(DataConfig *data)
{
	std::cout << "config_path = " << data->config_path << std::endl;

	open_conf(data);
//	print_vect(data->brut_line);
//	std::cout << std::endl;
	tockenize(data);
//	print_vect(data->token);
//	std::cout << std::endl;
	pars_state(data);
	std::cout << std::endl;

//	print_route(data->servers[0].routes[0]);
//	print_server(data->servers[0]);
	print_conf(data->servers);
}
