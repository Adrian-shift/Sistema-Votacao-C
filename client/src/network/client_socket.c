#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

#include "client_socket.h"

int connect_server(
    const char *ip,
    int port
)
{
    int sock;

    struct sockaddr_in server_addr;

    sock = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    if(sock < 0)
    {
        return -1;
    }

    server_addr.sin_family = AF_INET;

    server_addr.sin_port = htons(port);

    inet_pton(
        AF_INET,
        ip,
        &server_addr.sin_addr
    );

    if(connect(
        sock,
        (struct sockaddr*)&server_addr,
        sizeof(server_addr)
    ) < 0)
    {
        return -1;
    }

    return sock;
}

int send_vote(
    int socket_fd,
    const char *voter_id,
    const char *candidate,
    char *response
)
{
    char message[256];

    snprintf(
        message,
        sizeof(message),
        "%s %s",
        voter_id,
        candidate
    );

    send(
        socket_fd,
        message,
        strlen(message),
        0
    );

    int bytes = recv(
        socket_fd,
        response,
        255,
        0
    );

    if(bytes <= 0)
    {
        return 0;
    }

    response[bytes] = '\0';

    return 1;
}
