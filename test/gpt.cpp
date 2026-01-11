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

	int yes = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8081);
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
	return 0;
}
