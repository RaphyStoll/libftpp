#pragma once

#include <string>
#include <map>
#include <cstddef>

namespace libftpp {
namespace HttpRequest {

class HttpRequest {
private:
	HttpRequest(const HttpRequest& other);
	HttpRequest& operator=(const HttpRequest& other);

	std::string _method;	// "GET", "POST", ...
	std::string _target;	// "/path?x=1"
	std::string _version;	// "HTTP/1.1"
	std::map<std::string, std::string> _headers; // clés normalisées en lowercase

	static std::string normalizeHeaderKey(const std::string& key);
	static std::string normKey(const std::string& k);

public:
	HttpRequest();
	~HttpRequest();

	// Reset complet (réutilisable en keep-alive)
	// Vide méthode, target, version et headers.
	void clear();

	// Parse la Request-Line.
	// Format attendu : "METHOD TARGET VERSION" (ex: "GET /path HTTP/1.1")
	// Retourne true si succès, false si format invalide.
	bool parseRequestLine(const std::string& line);

	// Parse une ligne d'en-tête.
	// Format attendu : "Key: Value" (ex: "Host: example.com")
	// Retourne false si pas de ':' ou clé vide.
	// Note : Les clés sont stockées en lowercase. Les valeurs sont trimmées.
	// Politique doublons : écrase la valeur précédente (simplification).
	bool parseHeaderLine(const std::string& line);

	// Accesseurs (lecture seule)
	const std::string& getMethod() const;
	const std::string& getTarget() const;
	const std::string& getVersion() const;

	// --- Headers (case-insensitive via normalisation) ---
	
	// Vérifie si un header existe (clé insensible à la casse).
	bool hasHeader(const std::string& key) const;

	// Récupère la valeur d'un header.
	// Si absent, retourne 'def' (par défaut "").
	std::string getHeader(const std::string& key, const std::string& def = "") const;

	// Helper spécifique pour Content-Length.
	// Parse la valeur en size_t.
	// Retourne true si présent et valide (chiffres uniquement), false sinon.
	bool contentLength(size_t& out) const;
};
}
}
