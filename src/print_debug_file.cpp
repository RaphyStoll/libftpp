#include "debug.hpp"
#include "config.hpp"
#include <iostream>
#include <fstream>
#include <string>

/**
 * @brief Constructeur de la classe DebugLogger
 *
 * À la création d'une instance, le fichier de log est effacé (mode trunc)
 * pour démarrer avec un fichier propre.
 *
 * @param filename nom du fichier de log
 */
libftpp::debug::DebugLogger::DebugLogger(const std::string& filename) 
	: _filename(filename) {
	#ifdef DEBUG
		// Effacer le fichier à la création du logger
		std::ofstream outfile(_filename.c_str(), std::ios::trunc);
		outfile.close();
	#endif
}

/**
 * @brief Destructeur de la classe DebugLogger
 *
 * Ne fait rien de spécifique mais est fourni pour la clarté.
 */
libftpp::debug::DebugLogger::~DebugLogger() {
}

/**
 * @brief Ajoute un message dans le fichier de log (mode append)
 *
 * @param message message à écrire dans le fichier
 */
void libftpp::debug::DebugLogger::log(const std::string& message) {
	#ifdef DEBUG
		std::ofstream outfile(_filename.c_str(), std::ios::app);
		if (!outfile)
			return ;
        
		outfile << message << std::endl;
		outfile.close();
	#else
		(void)message;
	#endif
}

/**
 * @brief Efface le contenu du fichier de log (mode trunc)
 */
void libftpp::debug::DebugLogger::clear() {
	#ifdef DEBUG
		std::ofstream outfile(_filename.c_str(), std::ios::trunc);
		outfile.close();
	#endif
}

/**
 * @brief Méthode statique pour écrire dans `debug.log`
 *
 * Au premier appel le fichier est effacé, les appels suivants ajoutent des lignes
 * en fin de fichier.
 *
 * @param message message à écrire dans debug.log
 */
void libftpp::debug::DebugLogger::debug(const std::string& message) {
	#ifdef DEBUG
		static bool first = true;
        
		std::ios_base::openmode mode = std::ios::app;
		if (first) {
			mode = std::ios::trunc;
			first = false;
		}
        
		std::ofstream outfile("debug.log", mode);
		if (!outfile)
			return ;
        
		outfile << message << std::endl;
		outfile.close();
	#else
		(void)message;
	#endif
}
