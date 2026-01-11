#include <fstream>
#include <string>
#include <iostream>
#include <map>
#include <fstream>
#include <cstring>
#include <sstream>
#include <vector>
#include <cerrno>
#include <cstring>

#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>

#include "../../lib/LIBFTPP/include/libftpp.hpp"

struct Config {
	std::string	host;
	int			port;
	std::string	root;
	std::string	index;
	std::map<std::string, std::string> routes; //path -> files
};

static bool eventLoop(Config *config, int *server_fd);
static bool send_all(int fd, const std::string& data);
static std::string buildHttpResponse(int status, const std::string& body);
static bool resolveBody(const Config& cfg, const std::string& path, int& status, std::string& body);
static bool read_file(const std::string& path, std::string& out);
static std::string parsePathFromRequest(const std::string& req);
static bool readUntilHeadersDone(int fd, std::string& out);
static bool bootStrap(Config *config, int *server_fd);
static bool openFileAndParseConfig(const std::string &s, Config *config);
static void just_print(std::string s);
static void just_print_perror(const char *s);
static void just_print_error(std::string s);

int main () 
{
	Config config;
	std::string config_folder = "config";
	std::string config_file = "config.conf";
	std::string config_path = config_folder + "/" + config_file;
	int server_fd = -1; // default
	if (!openFileAndParseConfig(config_path, &config))
		return (just_print_error("open_file_and_parse_config failed"), 1);
	if (!bootStrap(&config, &server_fd)) {
		if (server_fd != -1)
			::close(server_fd);
		return (just_print_error("bootStrap failed"), 1);
	}
	if (!eventLoop(&config, &server_fd)) {
		if (server_fd != -1)
			::close(server_fd);
		return (just_print_error("event_loop failed"), 1);
	}
	return 0;
}

static void just_print_error(std::string s)
{
	std::cerr << s << std::endl;
}

static void just_print_perror(const char *s)
{
	std::cerr << s << ": " << std::strerror(errno) << std::endl;
}

static void just_print(std::string s)
{
	std::cerr << s << std::endl;
}

static bool openFileAndParseConfig(const std::string &s, Config *config)
{
	std::fstream file(s.c_str());
	if (!file.is_open()) {
		just_print_error("Cannot open " + s);
		return false;
	}

	std::string line;
	while (std::getline(file, line)) {
		if (line.empty())
			return(just_print_error("getline failed"), false);
		std::vector<std::string> split = libftpp::str::StringUtils::split(line, ' ', 0);
		if (split.size() < 2)
			return(just_print_error("split failed"), false);
		std::string key = libftpp::str::StringUtils::trim(split[0]);
		std::string value = libftpp::str::StringUtils::trim(split[1]);

		if (libftpp::str::StringUtils::iequals(key, "listen"))
			config->host = value;
		else if (libftpp::str::StringUtils::iequals(key, "port"))
			config->port = libftpp::str::StringUtils::stoi(value);
		else if (libftpp::str::StringUtils::iequals(key, "root"))
			config->root = value;
		else if (libftpp::str::StringUtils::iequals(key, "index"))
			config->index = value;
		else if (libftpp::str::StringUtils::iequals(key, "route")) {
			std::string path = libftpp::str::StringUtils::trim(split[1]);
			std::string file = libftpp::str::StringUtils::trim(split[2]);
			config->routes[path] = file;
			//just_print(path + " -> " + config.routes[path]);
		}
		else {
			std::string build_msg = "key not found : " + value;
			just_print_error(build_msg);
			return false;
		}
	}
	return true;
}

static bool bootStrap(Config *config, int *server_fd)
{
	*server_fd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (*server_fd < 0) {
		just_print_perror("socket");
		return false;
	}

	if (!libftpp::net::set_reuseaddr(*server_fd)) {
		just_print_perror("setsockopt");
		return false;
	}
	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(config->port);
	if (::inet_pton(AF_INET, config->host.c_str(), &addr.sin_addr) < 0) {
		just_print_perror("inet_pton");
		return false;
	}

	if (::bind(*server_fd, (sockaddr *)&addr, sizeof(addr)) < 0) {
		just_print_perror("bind");
		return false;
	}

	if (::listen(*server_fd, SOMAXCONN) < 0) {
		just_print_perror("listen");
		return false;
	}
	just_print("Listening on http://" + config->host + ":" + libftpp::str::StringUtils::itos(config->port));
	return true;
}

static bool readUntilHeadersDone(int fd, std::string& out) {
	libftpp::Buffer::Buffer buf;
	const size_t MAX = 16 * 1024; //16KB
	char temp[1024];

	while (true) {
		ssize_t n = ::recv(fd, temp, sizeof(temp), 0);

		if (n <= 0) {
			return false;
		}

		buf.append(temp, n);

		if (buf.size() > MAX)
			return false;
		
		if (buf.readUntilCRLFCRLF(out)) 
			return true;
	}

}

static std::string parsePathFromRequest(const std::string& req) {
	libftpp::Buffer::Buffer buf;
	buf.append(req);

	std::string line;
	if (!buf.readLineCRLF(line)) //extrait la 1ere ligne justa'au \r\n
		return "";

	libftpp::HttpRequest::HttpRequest request;
	if (!request.parseRequestLine(line)) // parser la ligne
		return "";
	
	if (request.getMethod() != "GET") // appliquer la regle
		return "";

	return request.getTarget(); // return path
}

static bool read_file(const std::string& path, std::string& out) {
	std::ifstream f(path.c_str(), std::ios::in);
	if (!f.is_open())
		return false;
	
	std::ostringstream oss;
	oss << f.rdbuf();
	out = oss.str();
	return true;
}

static bool resolveBody(const Config& cfg, const std::string& path, int& status, std::string& body) {
	std::string fullpath;

	if (path ==  "/") {
		status = 200;
		fullpath = cfg.root + "/" + cfg.index;
		if (!read_file(fullpath, body)) {
			status = 404;
			body = "<h1>404</h1><p>index.html missing</p>";
			return true;
		}
		return true;
	}

	// si pas index tout devient 404
	status = 404;
	if (!read_file("./www/errors/404.html", body)) {
		// Fallback
		body = "<!doctype html><html><body><h1>404</h1><p>Not Found (default)</p></body></html>";
	}
	return true;
}

static std::string buildHttpResponse(int status, const std::string& body) {
	std::ostringstream oss;

	if (status == 200)
		oss << "HTTP/1.1 200 OK\r\n";
	else
		oss << "HTTP/1.1 404 Not Found\r\n";
	
	oss << "Content-Type: text/html; charset=utf-8\r\n";
    oss << "Content-Length: " << body.size() << "\r\n";
    oss << "Connection: close\r\n";
    oss << "\r\n";
    oss << body;

	return oss.str();
}

static bool send_all(int fd, const std::string& data) {
	size_t total = 0;

	while (total < data.size()) {
		const char* p = data.c_str() + total;
		size_t left = data.size() - total;

		ssize_t n = ::send(fd, p, left, 0);
		if (n < 0)
			return false;
		total += static_cast<size_t>(n);
	}
	return true ;
}

static bool eventLoop(Config *config, int *server_fd)
{
	std::vector<pollfd> pollfds;
	pollfd s;
	std::memset(&s, 0, sizeof(s));
	
	s.fd = *server_fd;
	s.events = POLLIN;
	// s.revents deja set par memset
	// choix de conception -> toujours initialiser une struct le plus tot possible

	pollfds.push_back(s);
	while(true)
	{
		int ready = ::poll(&pollfds[0], pollfds.size(), -1);
		if (ready < 0) {
			just_print_perror("poll");
			return false;
		}

		size_t i = 0;
		while (i < pollfds.size()) {
			pollfd& p = pollfds[i];

			if (p.revents == 0) {
				++i;
				continue;
			}
			// event sur serveur fd = il faut accept le client
			if (i == 0) {
				int client_fd = ::accept(*server_fd, NULL, NULL);
				if (client_fd < 0) {
					just_print_perror("accept");
				}
				else {
					pollfd client;
					std::memset(&s, 0, sizeof(s));
					client.fd = client_fd;
					client.events =  POLLIN;
					pollfds.push_back(client);
				}

				p.revents = 0;
				++i;
				continue;
			}
			// event sur client fd
			int client_fd = p.fd;
			if (p.revents & (POLLERR | POLLHUP | POLLNVAL)) {
				::close(client_fd);
				pollfds[i] = pollfds.back();
				pollfds.pop_back();
				continue;
			}
			if (p.revents & POLLIN) {
				std::string req;
				bool ok = readUntilHeadersDone(client_fd, req);
				
				if (!ok) {
					::close(client_fd);
					pollfds[i] = pollfds.back();
					pollfds.pop_back();
					continue;
				}

				std::string path = parsePathFromRequest(req);

				int status = 404;
				std::string body;
				(void)resolveBody(*config, path, status, body);

				std::string resp = buildHttpResponse(status, body);

				send_all(client_fd, resp);

				pollfds[i] = pollfds.back();
				pollfds.pop_back();
				continue;
			}

			p.revents = 0;
			++i;
		}
	}
	return true;
}
