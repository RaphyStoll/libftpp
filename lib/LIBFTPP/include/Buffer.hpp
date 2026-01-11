#pragma once

#include <string>
#include <cstddef>

namespace libftpp {
namespace Buffer {

class Buffer {
	private:
	std::string _buf;   // stockage contient tout le buffer jusqu'a vider ou trop long (TODO: definir taille max)
	size_t	  _off;   // début de la fenêtre lisible dans _buf (comme on v§ide pas le buffer on veut savoir ou est le curseur)
	size_t	  _max_bytes;  // limite max sur la fenêtre lisible (0 = illimité)
	bool		_overflow;   // flag true = overflow

	void		compact_if_needed();
	public:
		Buffer(size_t max_bytes = 0);
		~Buffer();
		Buffer(const Buffer& other);
		Buffer& operator=(const Buffer& other);

		bool		empty() const;		// est-ce que la fenêtre de lecture lisible est vide ?
		size_t	  size() const;	 // nb d'octets disponibles dans la fenêtre lisible
		void		clear();		  // remet à zéro tout le buffer

		// Accès à la fenêtre lisible (pointeur invalide après append/consume/clear/compact)
		const char* data() const;

		// Définit une limite max (en octets) sur la fenêtre lisible.
		// max_bytes = 0 => ilimiter
		void		setMax(size_t max_bytes);
		size_t	  max() const;

		// Vrai si un append a tenté de dépasser la limite.
		bool		overflow() const;

		// Remet overflow à false (clear() le fait aussi).
		void		resetOverflow();

		// Ajoute les octets [p, p+n) à la fin du buffer
		// No-op si p == NULL ou n == 0
		void		append(const char* p, size_t n);
		void		append(const std::string& s);

		// Retire n octets du début de la fenêtre lisible
		// Si n >= size(), le buffer devient vide
		void		consume(size_t n);

		// Copie les n premiers octets dans out et les consomme
		// Retourne false si pas assez d'octets disponibles
		bool		take(size_t n, std::string& out);

		// Copie les n premiers octets dans out sans consommer
		// Retourne false si pas assez d'octets disponibles.
		bool		peek(size_t n, std::string& out) const;

		// Cherche 'needle' dans la fenêtre lisible.
		// Retourne la position dans [0..size()) ou std::string::npos.
		size_t	  find(const std::string& needle) const;

		// CRLF = \r\n
		// Si une ligne terminée par CRLF est disponible, la place dans out (sans CRLF),
		// consomme la ligne + CRLF, puis retourne true. Sinon retourne false.
		bool		readLineCRLF(std::string& out);

		// Si CRLFCRLF est disponible, place tout ce qui est avant dans out (sans délimiteur),
		// consomme le tout + CRLFCRLF, puis retourne true. Sinon retourne false.
		bool		readUntilCRLFCRLF(std::string& out);
};

}
}
