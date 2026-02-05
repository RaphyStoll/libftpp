#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../lib/LIBFTPP/include/libftpp.hpp"
#include "ConfigParser.hpp"
#include "RequestParser.hpp"
#include <string>

namespace webserv {
namespace core {

class Client {
public:
  // Constructeurs / Destructeur
  Client(int fd);
  Client();
  ~Client();
  void reset();

  // Getters
  int getFd() const;
  webserv::http::RequestParser &getParser();
  std::string &getResponseBuffer();

  // Gestion du Timeout
  void updateLastActivity();
  bool hasTimedOut(unsigned long long now_ms,
                   unsigned long long timeout_limit) const;

  // Gestion des données
  void appendResponse(const std::string &data);
  bool hasResponseToSend() const;
  void clearResponseBuffer();

private:
  int _fd;
  webserv::http::RequestParser _parser;
  libftpp::time::Timeout _last_activity;
  std::string _response_buffer;
};

} // namespace core
} // namespace webserv

#endif
