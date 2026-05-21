#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include "client_socket.h"

#define CONNECT_TIMEOUT_SECONDS 5

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

    if(inet_pton(
        AF_INET,
        ip,
        &server_addr.sin_addr
    ) != 1)
    {
        close(sock);
        return -1;
    }

    int flags = fcntl(sock, F_GETFL, 0);

    if(flags < 0)
    {
        close(sock);
        return -1;
    }

    if(fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        close(sock);
        return -1;
    }

    int result = connect(
        sock,
        (struct sockaddr*)&server_addr,
        sizeof(server_addr)
    );

    if(result < 0 && errno != EINPROGRESS)
    {
        close(sock);
        return -1;
    }

    if(result < 0)
    {
        fd_set write_fds;

        FD_ZERO(&write_fds);
        FD_SET(sock, &write_fds);

        struct timeval timeout;

        timeout.tv_sec = CONNECT_TIMEOUT_SECONDS;
        timeout.tv_usec = 0;

        result = select(
            sock + 1,
            NULL,
            &write_fds,
            NULL,
            &timeout
        );

        if(result <= 0)
        {
            close(sock);
            return -1;
        }

        int socket_error = 0;
        socklen_t socket_error_len = sizeof(socket_error);

        if(getsockopt(
            sock,
            SOL_SOCKET,
            SO_ERROR,
            &socket_error,
            &socket_error_len
        ) < 0 || socket_error != 0)
        {
            close(sock);
            return -1;
        }
    }

    if(fcntl(sock, F_SETFL, flags) < 0)
    {
        close(sock);
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
