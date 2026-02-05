#include "Post.cpp"
#include <string>

std::string webserv::http::Post::_getUploadPath(const std::sting& root, const std::string& reqPath, int& httpCode)
{

	// 1. trouver la racine du serveur pour avoir path abs
	// 2. build chemin cible
	// 3. check si ../.. si oui les enlever si non ok
	// 4. check si dossier = interdit, il faut que ce soit un fichier
	// 5. check si dossier parent as les droit W_OK
	
	// ex: si /uploads/images/img.png
	// 1. build ./uploads/images/img.png ou autre si sous dossier dans la config
	// 2. si post ../../uploads/images/img.png -> uploads/images/img.png ou erreur?
	// 3. si post uploads/images = dossier = error: bad request
	// 4. check si uploads et images ont les droit W_OK

	// si erreur return ""
	// si reussi return full path
	char resolvedRoot[PATH_MAX];
	if (realpath(root.c_str, resolvedRoot) == NULL) {
		httpCode = 500;
		return "";
	}
	std::string absRoot = resolvedRoot;
	std::string fullPath = absRoot + reqPath;

	if (fullPath.find(absRoot) != 0) {
		httpCode = 403;
		return "";
	}
	if (fullPath.size() - 1 == '/') {
		
	}
}