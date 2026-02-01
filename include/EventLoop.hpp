#ifndef EVENTLOOP_HPP
# define EVENTLOOP_HPP

# include "../lib/LIBFTPP/include/libftpp.hpp"
# include "RequestParser.hpp"
# include "Request.hpp"

# include <vector>
# include <map>
# include <string>

# include <poll.h>
# include "Config.hpp"


enum Method { ERROR, GET, DELETE, POST };
namespace webserv {
	
	
	/**
	 * @brief Gestionnaire principal de la boucle d'événements (Reactor Pattern
	 * 
	 * Cette classe utilise poll() pour surveiller à la fois les sockets (nouvelles connexions)
	 * et les sockets clients (données entrantes)
	 * Elle distribue les événements aux gestionnaires appropriés (handle, accept, read, close)
	 */
	class EventLoop {
		public:
			EventLoop(const std::vector<int>& listen_sockets, const NetworkConfig& config);
			~EventLoop();

			// Boucle principale
			void run();

		private:
			libftpp::debug::DebugLogger _logger;
			NetworkConfig _config;
			
			std::vector<int> _listen_sockets;
			
			// Le tableau de structures pour poll()
			std::vector<struct pollfd> _poll_fds;

			// Map pour stocker l'état du parsing pour chaque client (fd -> parser)
			std::map<int, http::RequestParser> _client_parsers;

			// map pour stocker les buffer de reponse en attente d'etre envoyer (fd -> data a envoyer)
			std::map<int, std::string> _write_buffers;

			// Initialise le vecteur _poll_fds avec les sockets d'écoute
			void _setup_initial_poll_fds();

			// Gestionnaires d'événements
			void _handle_poll_events();
			void _accept_new_connection(int listen_fd);
			void _handle_client_data(int client_fd, size_t poll_index);
			void _close_connection(int fd, size_t poll_index);

			//======  helper  ======

			// transforme la RequestParser::Request std::string _method
			// en enum GET, DELET, POST pour switch
			
			std::string _runGetMethod(const http::Request &req, const ServerConfig &srvConfig);
			std::string _runDeletMethod(const http::Request &req, const ServerConfig &srvConfig);
			std::string _runPostMethod(const http::Request &req, const ServerConfig &srvConfig);
			std::string _runErrorMethod(const http::Request &req, const ServerConfig &srvConfig);
			
			Method _toEnum(const std::string &s);
			
			// Récupère la bonne config serveur en fonction du port et du Header Host
			const ServerConfig& _getServerConfig(int client_fd, const http::Request& req);
			std::string _getContentType(const std::string& path);
			std::string _readFile(const std::string& path);
			std::string _generateErrorResponse(int code, const std::string& msg, const ServerConfig& config);
		};
}



#endif
