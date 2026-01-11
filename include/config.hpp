#ifndef CONFIG_HPP
# define CONFIG_HPP

/**
 * @file config.hpp
 * @brief Configuration de compilation pour les modules (macro DEBUG)
 *
 * Définit la macro `DEBUG` si elle n'est pas déjà définie. Cette macro est
 * utilisée dans les fonctions de logging pour activer/désactiver l'écriture
 * des fichiers de debug.
 */

#ifndef DEBUG
# define DEBUG 1
#endif

#endif