#include "../../../include/EventLoop.hpp"

using namespace webserv;

Method EventLoop::_toEnum(const std::string &s)
{
	if (s == "GET")
		return GET;
	else if (s == "DELET")
		return DELETE;
	else if (s == "POST")
		return POST;
	else
		return ERROR;
}