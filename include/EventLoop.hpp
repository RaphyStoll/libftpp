#ifndef EVENTLOOP_HPP
# define EVENTLOOP_HPP

# include <vector>
# include <poll.h>
# include <map>
# include "RequestParser.hpp"
# include "Request.hpp"
# include "../lib/LIBFTPP/include/libftpp.hpp"


enum Method { ERROR, GET, DELET, POST };
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
			EventLoop(const std::vector<int>& listen_sockets);
			~EventLoop();

			// Boucle principale
			void run();

		private:
			libftpp::debug::DebugLogger _logger;
			
			std::vector<int> _listen_sockets;
			
			// Le tableau de structures pour poll()
			std::vector<struct pollfd> _poll_fds;

			// Map pour stocker l'état du parsing pour chaque client (fd -> parser)
			std::map<int, http::RequestParser> _client_parsers;

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
			
			bool runGetMethod(const http::Request &req);
			bool runDeletMethod(const http::Request &req);
			bool runPostMethod(const http::Request &req);
			Method toEnum(const std::string &s);
		};
}



#endif
