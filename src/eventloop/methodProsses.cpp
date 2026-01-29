#include "../../include/EventLoop.hpp"
using namespace webserv;

bool EventLoop::runGetMethod(const http::Request &req)
{
	(void)req;
	//return printf("runGetMethod ok");
	std::cout << ("runGetMethod ok") << std::endl;
	
	return(0);
	
}

bool EventLoop::runDeletMethod(const http::Request &req)
{
	(void)req;
	//return printf("runDeletMethod ok");
	std::cout << ("runDeletMethod ok") << std::endl;
	return(0);
}

bool EventLoop::runPostMethod(const http::Request &req)
{
	(void)req;
	std::cout << ("runPostMethod ok") << std::endl;
	//return printf("runPostMethod ok");
	return(0);
}