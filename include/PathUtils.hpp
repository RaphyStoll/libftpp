#pragma once

#include <string>
#include <vector>

namespace libftpp {
	namespace str {

		class PathUtils {
			public:
			    // Combine le root et le path de la requête en nettoyant les /../ et les //
			    // Retourne un chemin "propre" prêt pour access() ou stat()
			    static std::string join(const std::string& root, const std::string& reqPath);
			
			    // Vérifie si le fichier/dossier existe
			    static bool exists(const std::string& path);
			
			    // Vérifie si c'est un dossier
			    static bool isDirectory(const std::string& path);
			
			private:
			    PathUtils() {}
		};

	} // namespace path
} // namespace libftpp