#include <stdio.h>

#include <unistd.h>

#include <newt.h>

#include "ui/login.h"

#include "ui/voting.h"

#include "ui/receipt.h"

#include "network/client_socket.h"

int main()
{
    char ip[64];

    int port;

    char voter_id[64];

    char candidate[64];

    char response[256];

    newtInit();

    newtCls();

    show_login(
        ip,
        &port,
        voter_id
    );

    int sock =
        connect_server(ip, port);

    if(sock < 0)
    {
        newtWinMessage(
            "Erro",
            "OK",
            "Falha ao conectar"
        );

        newtFinished();

        return 1;
    }

    show_voting_menu(candidate);

    if(send_vote(
        sock,
        voter_id,
        candidate,
        response
    ))
    {
        show_receipt(response);
    }
    else
    {
        newtWinMessage(
            "Erro",
            "OK",
            "Falha ao enviar voto"
        );
    }

    close(sock);

    newtFinished();

    return 0;
}
