#include "../../../include/EventLoop.hpp"

using namespace webserv;

Method EventLoop::toEnum(const std::string &s)
{
	if (s == "GET")
		return GET;
	else if (s == "DELET")
		return DELET;
	else if (s == "POST")
		return POST;
	else
		return ERROR;
}