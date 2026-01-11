#include "debug.hpp"

/**
 * @brief Fonction legacy pour compatibilité
 *
 * Délègue l'écriture au logger statique. Conserve l'interface existante
 * pour le code qui utilisait `print_debug()`.
 *
 * @param str message à écrire dans debug.log
 */
void print_debug(const std::string str)
{
	libftpp::debug::DebugLogger::debug(str);
}
