// simple example to handle sockets
// Author: 2021-03-09 Ingo Höft <Ingo@Hoeft-online.de>, last modify 2022-01-06

#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

static int close_socket(int pSock) {
    // int close(int fd);
    if (close(pSock) == -1) {
        std::cerr << "mysocket: error closing socket\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int mysocket(char* a_ipaddr, int a_port) {
    sockaddr_in local_addr{}, peer_addr{};

    // fill address structure
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(a_port);
    if (inet_pton(AF_INET, a_ipaddr, (in_addr_t*)&local_addr.sin_addr.s_addr) !=
        1) {
        std::cerr << "mysocket: error inet_pton\n";
        return EXIT_FAILURE;
    }

    // get socket file descriptor
    // int socket(int domain, int type, int protocol);
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cerr << "mysocket: error getting socket\n";
        return EXIT_FAILURE;
    }

    // bind a local address to the socket
    // int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    if (bind(sock, (sockaddr*)&local_addr, sizeof(local_addr)) == -1) {
        std::cerr << "mysocket: error bind to socket\n";
        close_socket(sock);
        return EXIT_FAILURE;
    }

    // listen on the socket
    // listen_error = listen(s->fd, SOMAXCONN);
    if (listen(sock, 15) == -1) {
        std::cerr << "mysocket: error listen on socket\n";
        close_socket(sock);
        return EXIT_FAILURE;
    }

    // accept connections from a peer
    // int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    socklen_t peer_addr_size = sizeof(peer_addr);
    std::cout
        << "mysocket:listening on socket and accepting... (finish with ^C)\n";

    int conn_fd = accept(sock, (sockaddr*)&peer_addr, &peer_addr_size);

    if (conn_fd == -1) {
        std::cerr << "mysocket: error accepting on socket\n";
        close_socket(sock);
        return EXIT_FAILURE;
    }

    // select incoming connection on a socket
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(conn_fd, &readSet);

    int no_fd = select(conn_fd + 1, &readSet, NULL, NULL, NULL);

    if (no_fd <= 0) {
        std::cerr
            << "mysocket: error selecting incoming connection on a socket\n";
        close_socket(conn_fd);
        close_socket(sock);
        return EXIT_FAILURE;
    }

    // receive data
    char buffer[1024]{};
    while (true) {

        ssize_t numBytes = recv(conn_fd, buffer, sizeof(buffer), 0);

        if (numBytes <= 0) {
            if (errno == EINTR) {
                // A signal was caught
                close_socket(conn_fd);
                return close_socket(sock);
            }
            std::cerr << "mysocket: error receiving data\n";
            close_socket(conn_fd);
            close_socket(sock);
            return EXIT_FAILURE;
        }
        std::cout << buffer;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage:   " << argv[0]
                  << " <local_ip_address> <port>\n"
                     "Example: "
                  << argv[0]
                  << " 127.0.0.1 22649\n"
                     "Then you can use 'nc' on another console to send data,\n"
                     "e.g.: nc localhost 22649\n";
        return -1;
    }

    return mysocket(argv[1], std::stoi(argv[2]));
}
