---
title: Mini-webserv (mini-sujet)
date: 5 janvier 2026
---

# Mini-webserv : mini serveur pour comprendre les sockets

Objectif : faire un **mini serveur HTTP/1.1** très simple pour comprendre les primitives réseau (socket/bind/listen/accept/recv/send), puis l’améliorer par petites étapes.

## 1) MVP (le plus simple possible)

### Ce que le serveur fait
- Écoute sur `127.0.0.1:8080`
- Accepte 1 client à la fois (bloquant)
- Lit la requête brute, l’affiche en stdout
- Renvoie une réponse HTTP valide (200 OK) avec un body texte
- Ferme la connexion

### Points importants (HTTP/1.1)
- Une réponse minimale doit contenir :
  - Status line (`HTTP/1.1 200 OK`)
  - Des headers
  - Une ligne vide (`\r\n`)
  - Le body
- En HTTP/1.1, il vaut mieux envoyer **`Content-Length`**.
- Pour simplifier : **`Connection: close`** (on ferme après la réponse).

## 2) Code de base (C++ – serveur bloquant mono-client)

> But : comprendre le flow socket → bind → listen → accept → recv/send.

```cpp
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>

int main() {
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		perror("socket");
		return 1;
	}

	// Permet de relancer vite sans attendre TIME_WAIT
	int yes = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);
	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

	if (bind(server_fd, (sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		return 1;
	}

	if (listen(server_fd, 128) < 0) {
		perror("listen");
		return 1;
	}

	std::cout << "Listening on http://127.0.0.1:8080" << std::endl;

	while (true) {
		int client_fd = accept(server_fd, NULL, NULL);
		if (client_fd < 0) {
			perror("accept");
			continue;
		}

		char buf[4096];
		ssize_t n = recv(client_fd, buf, sizeof(buf) - 1, 0);
		if (n > 0) {
			buf[n] = '\0';
			std::cout << "---- Request ----\n" << buf << std::endl;
		}

		std::string body = "Hello from mini webserv\n";
		std::string resp =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/plain; charset=utf-8\r\n"
			"Content-Length: " + std::to_string(body.size()) + "\r\n"
			"Connection: close\r\n"
			"\r\n" + body;

		send(client_fd, resp.c_str(), resp.size(), 0);
		close(client_fd);
	}

	// unreachable, mais OK pour un mini-test
	// close(server_fd);
	return 0;
}
```

### Test rapide
- Compiler : `c++ -Wall -Wextra -Werror mini_server.cpp -o mini_server`
- Lancer : `./mini_server`
- Tester : `curl -v http://127.0.0.1:8080/`

## 3) Roadmap (évolution par niveaux)

### Niveau A — lecture propre des headers
- Lire jusqu’à `\r\n\r\n` (fin de headers)
- Parser seulement : `METHOD`, `PATH`, `VERSION`

### Niveau B — multi-clients (le vrai apprentissage)
- Passer en sockets **non bloquants** (`fcntl(fd, F_SETFL, O_NONBLOCK)`)
- Utiliser `poll()` (ou `select()`) pour :
  - accepter plusieurs clients
  - lire/écrire sans bloquer

### Niveau C — routes très simples
- `GET /` -> 200 (texte)
- `GET /health` -> 200 (OK)
- `GET /file` -> servir un fichier local
- 404 si inconnu

### Niveau D — erreurs HTTP basiques
- 400 (requête invalide)
- 404 (not found)
- 405 (method not allowed)

### Niveau E — keep-alive (optionnel au début)
- Supporter `Connection: keep-alive`
- Gérer plusieurs requêtes sur la même connexion

## 4) Checklist “OK j’ai compris”
- Je sais expliquer la différence entre : socket / bind / listen / accept
- Je sais pourquoi `Content-Length` est important
- Je sais ce que change le non-bloquant et pourquoi on a besoin de `poll()`

---

Si vous voulez, on peut ensuite écrire la version `poll()` minimaliste (accept + read headers + respond) en gardant le code le plus petit possible.
