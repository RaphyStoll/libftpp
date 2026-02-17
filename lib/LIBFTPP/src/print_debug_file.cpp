#include "debug.hpp"
#include "config.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <cstdio>
#include <ctime> 

/**
 * @brief Constructeur de la classe DebugLogger
 *
 * À la création d'une instance, le fichier de log est effacé (mode trunc)
 * pour démarrer avec un fichier propre.
 *
 * @param filename nom du fichier de log
 */
libftpp::debug::DebugLogger::DebugLogger(const std::string& filename) {
	std::string name = filename;
	// Ajouter .log si absent
	if (name.length() < 4 || name.substr(name.length() - 4) != ".log")
		name += ".log";

	// Créer le dossier log s'il n'existe pas
	struct stat st;
	if (stat("log", &st) != 0)
		mkdir("log", 0600);

	_filename = "log/" + name;

	#ifdef DEBUG
		// Effacer le fichier à la création du logger
		// std::ofstream outfile(_filename.c_str(), std::ios::trunc); 
		// outfile.close();
        // Modification: On n'efface plus le fichier à chaque construction
        // pour éviter d'effacer les logs à chaque requête si le logger est
        // instancié localement (ex: dans une méthode Get).
	#endif
}

/**
 * @brief Constructeur de copie
 */
libftpp::debug::DebugLogger::DebugLogger(const DebugLogger& other) : _filename(other._filename) {
	_ss << other._ss.str();
}

/**
 * @brief Opérateur d'affectation
 */
libftpp::debug::DebugLogger& libftpp::debug::DebugLogger::operator=(const DebugLogger& other) {
	if (this != &other) {
		_filename = other._filename;
		_ss.str("");
		_ss << other._ss.str();
	}
	return *this;
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

		// Timestamp
		std::time_t now = std::time(NULL);
		char buffer[80];
		std::strftime(buffer, sizeof(buffer), "[%d-%m-%Y %H:%M:%S] ", std::localtime(&now));
		
		outfile << buffer << message << std::endl;
		outfile.close();

		// Ecriture dans general.log
		std::ofstream generalFile("log/general.log", std::ios::app);
		if (generalFile) {
			// On ajoute le nom du fichier d'origine pour savoir qui parle dans le general
			generalFile << buffer << "[" << _filename << "] " << message << std::endl;
			generalFile.close();
		}
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
 * @brief Supprime tous les fichiers .log du dossier log/
 */
void libftpp::debug::DebugLogger::cleanAll() {
	#ifdef DEBUG
		DIR *dir;
		struct dirent *ent;
		if ((dir = opendir("log")) != NULL) {
			while ((ent = readdir(dir)) != NULL) {
				std::string filename = ent->d_name;
				if (filename.length() > 4 && filename.substr(filename.length() - 4) == ".log") {
					std::string filepath = "log/" + filename;
					std::remove(filepath.c_str());
				}
			}
			closedir(dir);
		}
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

		// Timestamp
		std::time_t now = std::time(NULL);
		char buffer[80];
		std::strftime(buffer, sizeof(buffer), "[%d-%m-%Y %H:%M:%S] ", std::localtime(&now));
		
		outfile << buffer << message << std::endl;
		outfile.close();

		// Ecriture dans general.log
		std::ofstream generalFile("log/general.log", std::ios::app);
		if (generalFile) {
			generalFile << buffer << "[static debug] " << message << std::endl;
			generalFile.close();
		}
	#else
		(void)message;
	#endif
}
