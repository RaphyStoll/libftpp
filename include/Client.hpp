#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "RequestParser.hpp"
#include "../lib/LIBFTPP/include/Clock.hpp"
#include "ConfigParser.hpp"
#include <string>

namespace webserv {
	class Client {
	public:
	    Client(int fd, const webserv::ServerConfig& default_config);
	    ~Client();

	    // Gestion du socket
	    int getFd() const;
	
	    // Gestion du Timeout
	    void updateLastActivity();
	    bool hasTimedOut(unsigned long long timeout_ms) const;

	    // Gestion des données (Lecture/Écriture)
	    void appendResponse(const std::string& data);
	    bool hasResponseToSend() const;
	    std::string& getResponseBuffer(); // ou une méthode sendChunk()
	
	    // Accès au parser pour l'EventLoop
	    http::RequestParser& getParser();

	private:
	    int _fd;
	    http::RequestParser _parser;
	    libftpp::time::Timeout _last_activity;
	    std::string _response_buffer;
	
	    // Optionnel : Si tu veux garder la config associée au client
	    // const webserv::ServerConfig& _config; 
	};
}
#endif