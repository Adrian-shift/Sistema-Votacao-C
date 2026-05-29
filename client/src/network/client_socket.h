#ifndef CLIENT_SOCKET_H
#define CLIENT_SOCKET_H

typedef struct {
    int socket_fd;
    void* ssl;
    void* ssl_ctx;
} ssl_connection_t;

ssl_connection_t* connect_server_ssl(
    const char *ip,
    int port
);

void close_ssl_connection(ssl_connection_t* conn);

int send_vote(
    ssl_connection_t* conn,
    const char *voter_id,
    const char *candidate,
    char *response
);

#endif
