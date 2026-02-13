# include "ConfigParser.hpp"

DataConfig::DataConfig() : 	_i(0), _state(GLOBAL)
{

};

DataConfig::~DataConfig()
{

};

static void suppr_comment(std::string &line)
{
	size_t commentPos = line.find('#');

	if(commentPos != std::string::npos)
		line = line.substr(0, commentPos);

	line = libftpp::str::StringUtils::trim(line);
}

void DataConfig::openConf()
{
	std::ifstream file(config_path.c_str(), std::ios::in);
	if (!file.is_open())
	{
		throw std::runtime_error("\"" + config_path + "\" introuvable");
	}

	std::string line;
	while( std::getline(file, line))
	{
		suppr_comment(line);
		if(!line.empty())
			_brutLine.push_back(line);
	}

	if(_brutLine.empty()) //SDU inutil, tester + tard??
	{
		throw std::runtime_error("fichier de config vide");
	}
}

void DataConfig::tockenize()
{
	std::string line;
	std::string word;
	char c;

	size_t i;
	size_t j;
	
	i = 0;
	while(i < _brutLine.size())
	{
		word.clear();
		line = _brutLine[i];
		j = 0;
		while(j < line.size())
		{
			c = line[j];
			if(c == ' ')
			{
				if(! word.empty())
					_token.push_back(word);
				word.clear();
			}
			else if (c == '{' || c == '}')
			{
				if(! word.empty())
					_token.push_back(word);
				word.clear();
				word += c;
				if(! word.empty())
					_token.push_back(word);
				word.clear();
			}
			else
			{
				word += c;
			}
			j++;
		}
		if(! word.empty())
			_token.push_back(word);
		i++;
	}
}

void DataConfig::pars_state()
{
//	_i = 0;
//	_state = GLOBAL;


	while(_i < _token.size()) //SDU passer en case?
	{
		if(_state == GLOBAL)
		{
			state_global();
		}
		else if(_state == IN_SERVER)
		{
			state_server();
		}
		if(_state == IN_ROUTE)
		{
			state_route();
		}
	}

	if (_state != GLOBAL)
		throw std::runtime_error("block {} unclosed");

}

NetworkConfig DataConfig::cParser()
{
	std::cout << "config_path = " << config_path << std::endl; //SDU

	openConf();
//	print_vect(_brutLine); //SDU
//	std::cout << std::endl; //SDU
	tockenize();
//	print_vect(_token); //SDU
//	std::cout << std::endl; //SDU
	pars_state();
	std::cout << std::endl;
//	_servers.print(); //SDU

	NetworkConfig net_tmp;
	for(size_t i = 0; i < _servers.size(); i++)
	{
		net_tmp[_servers[i].port].push_back(_servers[i]);
	}
 	return net_tmp;

}
