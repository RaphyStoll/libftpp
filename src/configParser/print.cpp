# include "ConfigParser.hpp"

void print_vect(const std::vector<std::string> &vect)
{
	size_t i = 0;
	while(i < vect.size())
	{
		std::cout << "vect[" << i << "] : " << vect[i] << std::endl;
		i++;
	}
}

void print_conf (const std::vector<ServerConfig> &serv)
{
	size_t i;

	std::cout << "****** Conf ******" << std::endl;
	std::cout << std::endl;

	i = 0;
	while(i < serv.size())
	{
		std::cout << "Server " << i << " :" << std::endl;
		serv[i].print();
		i++;
	}
	std::cout << std::endl;
}