#include <unistd.h>

#include "Client.hpp"

#define NEW_CONNECTION_TIMEOUT 60000 // 1000 = 1s


using namespace webserv::core;

webserv::core::Client::Client(int fd) : _fd(fd), _last_activity(NEW_CONNECTION_TIMEOUT)
{
	_last_activity.touch(libftpp::time::Clock::now_ms());
}

webserv::core::Client::Client() : _fd(-1), _last_activity(NEW_CONNECTION_TIMEOUT) {}

webserv::core::Client::~Client() {}

int webserv::core::Client::getFd() const { return _fd; }

//webserv::http::RequestParser& webserv::core::Client::getParser() { return _parser; }
http::RequestParser& webserv::core::Client::getParser() { return _parser; }

std::string& webserv::core::Client::getResponseBuffer() { return _response_buffer; }

// --- Gestion du Timeout ---

void webserv::core::Client::updateLastActivity()
{
	_last_activity.touch(libftpp::time::Clock::now_ms());
}

bool webserv::core::Client::hasTimedOut(unsigned long long now_ms, unsigned long long timeout_limit) const
{
	(void)timeout_limit; 
	return _last_activity.expired(now_ms);
}

// --- Gestion des données ---

void webserv::core::Client::appendResponse(const std::string& data)
{
	_response_buffer += data;
}

bool webserv::core::Client::hasResponseToSend() const { return !_response_buffer.empty(); }

void webserv::core::Client::clearResponseBuffer()
{
	_response_buffer.clear();
}

void webserv::core::Client::reset()
{
	_response_buffer.clear();
	// TODO RAPH:
	// a use une fois implementer par seb
	//_parser.reset(); 
	updateLastActivity();
}