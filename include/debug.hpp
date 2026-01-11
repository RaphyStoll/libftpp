#pragma once

#include "config.hpp"
#include <string>

namespace libftpp {
namespace debug {

/**
 * @brief Classe simple pour gérer un fichier de log
 *
 * Chaque instance représente un fichier de log. À la création le fichier
 * est effacé (mode trunc), les appels successifs à `log()` ajoutent des lignes
 * à la fin du fichier (mode append). La méthode `clear()` permet d'effacer
 * explicitement le contenu du fichier.
 */
class DebugLogger {
	private:
		std::string _filename;

	public:
		/**
		 * @brief Constructeur
		 * @param filename nom du fichier de log
		 */
		DebugLogger(const std::string& filename);

		// @brief Destructeur
		~DebugLogger();
        
		/**
		 * @brief Ajoute un message dans le fichier de log (append)
		 * @param message message à écrire
		 */
		void log(const std::string& message);

		// Efface le contenu du fichier de log (mode trunc)
		void clear();
        
		/**
		 * @brief Méthode statique pour écrire dans le fichier global `debug.log`
		 *
		 * À la première utilisation le fichier est effacé, puis les appels
		 * suivants ajoutent des lignes en fin de fichier.
		 * @param message message à écrire
		 */
		static void debug(const std::string& message);
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
