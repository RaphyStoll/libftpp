#pragma once

#include "config.hpp"
#include <string>
#include <sstream>
#include <iostream>

namespace libftpp {
namespace debug {

/**
 * @brief Classe simple pour gérer un fichier de log
 *
 * Supporte désormais l'opérateur de flux <<.
 * Usage: logger << "Message " << 123 << std::endl;
 * Le fichier est écrit uniquement lors de l'appel à std::endl.
 */
class DebugLogger {
	private:
		std::string _filename;
		std::stringstream _ss;

	public:
		/**
		 * @brief Constructeur
		 * @param filename nom du fichier de log
		 */
		DebugLogger(const std::string& filename);
		DebugLogger(const DebugLogger& other);
		DebugLogger& operator=(const DebugLogger& other);
		~DebugLogger();
		
		void log(const std::string& message);
		void clear();
		
		static void debug(const std::string& message);

		// Template pour gérer tous les types (int, string, etc.)
		template <typename T>
		DebugLogger& operator<<(const T& value) {
			#ifdef DEBUG
				_ss << value;
			#endif
			return *this;
		}

		// Surcharge pour gérer std::endl
		// Cela permet: logger << "msg" << std::endl; (qui déclenche l'écriture dans le fichier)
		DebugLogger& operator<<(std::ostream& (*manip)(std::ostream&)) {
			#ifdef DEBUG
				if (manip == (std::ostream& (*)(std::ostream&))std::endl) {
					this->log(_ss.str());
					_ss.str("");   // Vider le buffer
					_ss.clear();   // Réinitialiser les flags d'erreur
				} else {
					_ss << manip;
				}
			#endif
			return *this;
		}
};

/**
 * @brief Fonction legacy pour compatibilité
 *
 * Appelle `DebugLogger::debug()` en interne. Préservée pour le code existant.
 * @param str message à écrire dans `debug.log`
 */
void print_debug(const std::string str);

}
}
