#include <unistd.h>

#include "Client.hpp"

#include "RequestParser.hpp"

#define NEW_CONNECTION_TIMEOUT 60000 // 1000 = 1s


using namespace webserv::core;
using namespace webserv;

webserv::core::Client::Client(int fd) : _logger("client"), _fd(fd), _last_activity(NEW_CONNECTION_TIMEOUT)
{
	//_logger << "New Client connection on fd: " << fd << std::endl;
	_last_activity.touch(libftpp::time::Clock::now_ms());
}

webserv::core::Client::Client() : _logger("client"), _fd(-1), _last_activity(NEW_CONNECTION_TIMEOUT) {
	//_logger << "Default Client constructor" << std::endl;
}

webserv::core::Client::~Client() {
	//_logger << "Client destructor called for fd: " << _fd << std::endl;
}

int webserv::core::Client::getFd() const { 
	//_logger << "getFd called: " << _fd << std::endl;
	return _fd; 
}

//webserv::http::RequestParser& webserv::core::Client::getParser() { return _parser; }
http::RequestParser& webserv::core::Client::getParser() { 
	//_logger << "getParser called" << std::endl;
	return _parser; 
}

std::string& webserv::core::Client::getResponseBuffer() { 
	//_logger << "getResponseBuffer called" << std::endl;
	return _response_buffer; 
}

// --- Gestion du Timeout ---

void webserv::core::Client::updateLastActivity()
{
	//_logger << "Updating last activity for fd: " << _fd << std::endl;
	_last_activity.touch(libftpp::time::Clock::now_ms());
}

bool webserv::core::Client::hasTimedOut(unsigned long long now_ms, unsigned long long timeout_limit) const
{
	(void)timeout_limit; 
	bool timedOut = _last_activity.expired(now_ms);
	if (timedOut)
		_logger << "Client on fd: " << _fd << " has timed out" << std::endl;
	return timedOut;
}

// --- Gestion des données ---

void webserv::core::Client::appendResponse(const std::string& data)
{
	_logger << "Appending " << data.size() << " bytes to response buffer for fd: " << _fd << std::endl;
	_response_buffer += data;
}

bool webserv::core::Client::hasResponseToSend() const { 
	_logger << "Checking if response to send for fd: " << _fd << ": " << (!_response_buffer.empty()) << std::endl;
	return !_response_buffer.empty(); 
}

void webserv::core::Client::clearResponseBuffer()
{
	_logger << "Clearing response buffer for fd: " << _fd << std::endl;
	_response_buffer.clear();
}

void webserv::core::Client::reset()
{
	_logger << "Resetting client on fd: " << _fd << std::endl;
	_response_buffer.clear();
	// TODO RAPH:
	// a use une fois implementer par seb
	_parser.reset(); 
	updateLastActivity();
}