#ifndef CLIENT_SOCKET_H
#define CLIENT_SOCKET_H

int connect_server(
    const char *ip,
    int port
);

int send_vote(
    int socket_fd,
    const char *voter_id,
    const char *candidate,
    char *response
);

#endif
