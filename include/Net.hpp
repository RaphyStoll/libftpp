#pragma once

#include <string>
#include <cstddef>

// POSIX / sockets
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace libftpp {
namespace net {

// -----------------------------------------------------------------------------
// Wrappers (retournent true si OK, false si erreur ; errno est laissé tel quel)
// -----------------------------------------------------------------------------
bool set_non_blocking(int fd);
bool set_reuseaddr(int fd);
bool set_nodelay(int fd); // optionnel : TCP_NODELAY (utile surtout sur TCP interactif)

// Convertit une sockaddr (IPv4/IPv6) en string.
// Exemples: "127.0.0.1:8080" / "[::1]:8080" / "192.168.0.10"
std::string sockaddr_to_string(const struct sockaddr* sa, socklen_t salen, bool with_port = true);

// -----------------------------------------------------------------------------
// RAII : Fd (close automatique)
// -----------------------------------------------------------------------------
class Fd {
private:
	int _fd;

	Fd(const Fd& other);
	Fd& operator=(const Fd& other);

public:
	Fd();				 // fd = -1
	explicit Fd(int fd);  // prend ownership
	~Fd();				// close si valid

	bool valid() const;
	int  get() const;

	// Remplace le fd actuel (ferme l'ancien si besoin)
	void reset(int fd = -1);

	// Donne le fd à l'appelant (ne fermera plus)
	int  release();
};
}
}
