#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <pthread.h>

#include "../models/server_state.h"

#include "../database/database.h"

#include "../protocol/protocol.h"

#define PORT 8080

void* handle_client(void* arg)
{
    int client_socket = *(int*)arg;

    free(arg);

    char buffer[1024];

    int bytes;

    server_state.connected_clients++;

    add_log("\n[INFO] Novo cliente conectado");

	while((bytes = recv(client_socket, buffer, sizeof(buffer)-1, 0)) > 0)
	{
		buffer[bytes] = '\0';

		char voter_id[64];

		char candidate[64];

		sscanf(
			buffer,
			"%63s %63s",
			voter_id,
			candidate
		);

		char log[256];

		snprintf(
			log,
			sizeof(log),
			"[VOTO] Eleitor %s -> %s",
			voter_id,
			candidate
		);

		add_log(log);

		if(!voter_exists(voter_id))
		{
			send(
				client_socket,
				"NACK ELEITOR_INVALIDO\n",
				23,
				0
			);

			continue;
		}

		if(has_voted(voter_id))
		{
			send(
				client_socket,
				"NACK JA_VOTOU\n",
				16,
				0
			);

			continue;
		}

		char receipt[128];

		generate_receipt(receipt);

		if(save_vote(
			voter_id,
			candidate,
			receipt
		))
		{
			server_state.total_votes++;

			char response[256];

			snprintf(
				response,
				sizeof(response),
				"ACK %s\n",
				receipt
			);

			send(
				client_socket,
				response,
				strlen(response),
				0
			);

			add_log("[INFO] Voto registrado");
		}
		else
		{
			send(
				client_socket,
				"NACK ERRO_DB\n",
				14,
				0
			);
		}
	}

    close(client_socket);

    server_state.connected_clients--;

    add_log("[INFO] Cliente desconectado");

    return NULL;
}

void* start_server(void* arg)
{
    int server_socket;

    struct sockaddr_in server_addr;

    server_socket = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    if(server_socket < 0)
    {
        add_log("[ERRO] Falha ao criar socket");
        return NULL;
    }

    server_addr.sin_family = AF_INET;

    server_addr.sin_port = htons(PORT);

    server_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(
        server_socket,
        (struct sockaddr*)&server_addr,
        sizeof(server_addr)
    ) < 0)
    {
        add_log("[ERRO] Bind falhou");
        return NULL;
    }

    if(listen(server_socket, 10) < 0)
    {
        add_log("[ERRO] Listen falhou");
        return NULL;;
    }

    add_log("[INFO] Servidor TCP iniciado");

    while(1)
    {
        int client_socket;

        struct sockaddr_in client_addr;

        socklen_t client_len = sizeof(client_addr);

        client_socket = accept(
            server_socket,
            (struct sockaddr*)&client_addr,
            &client_len
        );

        if(client_socket < 0)
        {
            add_log("[ERRO] Accept falhou");
            continue;
        }

        pthread_t client_thread;

        int* pclient = malloc(sizeof(int));

        *pclient = client_socket;

        pthread_create(
            &client_thread,
            NULL,
            handle_client,
            pclient
        );

        pthread_detach(client_thread);
    }
    return NULL;
}
