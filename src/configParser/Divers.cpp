# include "ConfigParser.hpp"

//SDU pour les tests, pourra etre supprimee
void print_vect(const std::vector<std::string> &vect)
{
	size_t i = 0;
	while(i < vect.size())
	{
		std::cout << "vect[" << i << "] : " << vect[i] << std::endl;
		i++;
	}
}