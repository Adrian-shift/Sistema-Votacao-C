#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <pthread.h>

#include "../models/server_state.h"

#include "../database/database.h"

#include "../protocol/protocol.h"

#include "ssl_wrapper.h"

#define PORT 8080
#define CERT_FILE "certs/server.crt"
#define KEY_FILE "certs/server.key"

typedef struct {
    int client_socket;
    SSL_CTX* ssl_ctx;
} client_args_t;

void* handle_client(void* arg)
{
    client_args_t* args = (client_args_t*)arg;
    int client_socket = args->client_socket;
    SSL_CTX* ssl_ctx = args->ssl_ctx;
    free(args);

    SSL* ssl = ssl_accept(ssl_ctx, client_socket);
    if(!ssl)
    {
        close(client_socket);
        add_log("[ERRO] Falha no handshake SSL");
        return NULL;
    }

    char buffer[1024];
    int bytes;

    server_state.connected_clients++;

    add_log("\n[INFO] Novo cliente conectado (SSL)");

	while((bytes = ssl_recv(ssl, buffer, sizeof(buffer)-1)) > 0)
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
			ssl_send(
				ssl,
				"NACK ELEITOR_INVALIDO\n",
				23
			);

			continue;
		}

		if(has_voted(voter_id))
		{
			ssl_send(
				ssl,
				"NACK JA_VOTOU\n",
				16
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

			ssl_send(
				ssl,
				response,
				strlen(response)
			);

			add_log("[INFO] Voto registrado");
		}
		else
		{
			ssl_send(
				ssl,
				"NACK ERRO_DB\n",
				14
			);
		}
	}

    ssl_cleanup(ssl, NULL);
    close(client_socket);

    server_state.connected_clients--;

    add_log("[INFO] Cliente desconectado");

    return NULL;
}

void* start_server(void* arg)
{
    SSL_CTX* ssl_ctx = init_server_ssl(CERT_FILE, KEY_FILE);
    if(!ssl_ctx)
    {
        add_log("[ERRO] Falha ao inicializar SSL");
        return NULL;
    }

    add_log("[INFO] SSL/TLS inicializado");

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
        ssl_cleanup(NULL, ssl_ctx);
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
        close(server_socket);
        ssl_cleanup(NULL, ssl_ctx);
        return NULL;
    }

    if(listen(server_socket, 10) < 0)
    {
        add_log("[ERRO] Listen falhou");
        close(server_socket);
        ssl_cleanup(NULL, ssl_ctx);
        return NULL;
    }

    add_log("[INFO] Servidor TCP iniciado na porta 8080 (SSL/TLS)");

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

        client_args_t* args = malloc(sizeof(client_args_t));
        args->client_socket = client_socket;
        args->ssl_ctx = ssl_ctx;

        pthread_create(
            &client_thread,
            NULL,
            handle_client,
            args
        );

        pthread_detach(client_thread);
    }

    close(server_socket);
    ssl_cleanup(NULL, ssl_ctx);
    return NULL;
}
