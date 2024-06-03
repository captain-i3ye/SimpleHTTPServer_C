#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


int main(void) {
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

	int server_fd;
	
	// create socket and store file descriptor
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		fprintf(stderr, "Socket creation failed ... %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	// TODO: set REUSE_PORT : Address already in use.
	int reuse = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) {
		fprintf(stderr, "SO_REUSEPORT failed ... %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	// set server address
	struct sockaddr_in serv_addr =  {   .sin_family = AF_INET,
										.sin_port = htons(4234),
										.sin_addr = { htonl(INADDR_ANY) },
									};

	// bind the socket
	if (bind(server_fd, (struct sockaddr *) &serv_addr, (socklen_t) sizeof(serv_addr)) != 0) {
		fprintf(stderr, "Could not bind socket ... %s\n", strerror(errno));
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	int connection_backlog = 5;
	// listen
	if (listen(server_fd, connection_backlog) != 0) {
		fprintf(stderr, "Listen failed ... %s\n", strerror(errno));
		close(server_fd); 
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in client_addr;
	int client_addr_len = sizeof(client_addr);
	int client_fd;
	
	printf("Waiting for connection ... \n");

	// accept the connection
	if ((client_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len)) < 0) {
		fprintf(stderr, "Failed to accept ... %s\n", strerror(errno));
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	printf("\n\nConnected on port %d\n", serv_addr.sin_port);

	char* response = "HTTP/1.1 200 OK\r\n\r\n";
	int bytes_sent = send(client_fd, response, strlen(response), 0);
	

	// close the connection
	close(server_fd);

	return 0;
}	
