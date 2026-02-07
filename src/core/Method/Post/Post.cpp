#include <string>
#include <iostream>

#include "Post.hpp"
#include "ResponseBuilder.hpp"



std::string webserv::http::Post::execute(const ::http::Request& req, const ServerConfig& server, const RouteConfig& route)
{
int httpCode = 201; // Par défaut : Created

    // 1. Vérification de la taille (Max Body Size du Serveur)
    if (!_checkBodySize(req.getBody().size(), server.max_body_size, httpCode)) {
        return ResponseBuilder::generateError(httpCode, server);
    }

    // 2. Calcul du chemin de destination
    // On passe la route et le serveur pour décider (upload_path vs root)
    std::string fullPath = _getUploadPath(req.getPath(), route, server, httpCode);
    
    if (httpCode != 201) { 
        // Si erreur dans le calcul du chemin (403 Forbidden, 500 Internal...)
        return ResponseBuilder::generateError(httpCode, server);
    }

    // 3. Vérification d'existence (200 OK vs 201 Created)
    // Si le fichier existe déjà, on l'écrase -> c'est une mise à jour (200)
    if (access(fullPath.c_str(), F_OK) == 0) {
        httpCode = 200;
    }

    // 4. Écriture du fichier
    if (!_writeFile(fullPath, req.getBody())) {
        return ResponseBuilder::generateError(500, server); // Erreur I/O
    }

    // 5. Construction de la réponse
    std::ostringstream response;
    
    response << "HTTP/1.1 " << httpCode << " " << ResponseBuilder::getStatusMessage(httpCode) << "\r\n";
    
    // Header Location (Standard HTTP pour une création 201)
    if (httpCode == 201) {
        response << "Location: " << req.getPath() << "\r\n";
    }
    
    response << "Content-Length: 0\r\n";
    response << "Connection: close\r\n"; // Ou keep-alive plus tard
    response << "\r\n";

	bool hardcode = 1; // FIXME : test hardcodder en attendant les cgi
	if (hardcode == 1)
	{
		std::string body1 = "<html><body><h1>POST Success!</h1><p>Donnees recues avec succes.</p></body></html>";
		std::ostringstream response1;
		response1 << "HTTP/1.1 200 OK\r\n";
		response1 << "Content-Type: text/html\r\n";
		response1 << "Content-Length: " << body1.length() << "\r\n";
		response1 << "Connection: close\r\n";
		response1 << "\r\n";
		response1 << body1;

		return response1.str();
	}
    return response.str();
}