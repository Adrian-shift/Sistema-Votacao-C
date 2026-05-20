#include <newt.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "../models/server_state.h"
#include "../network/server_socket.h"
#include "dashboard.h"

static int running = 1;

void* simulation_thread(void* arg)
{
    int client_id = 100;

    while(running)
    {
        sleep(3);

        server_state.connected_clients++;

        char log[256];

        sprintf(
            log,
            "[INFO] Cliente %d conectado",
            client_id
        );

        add_log(log);

        sleep(2);

        server_state.total_votes++;

        sprintf(
            log,
            "[INFO] Voto registrado do cliente %d",
            client_id
        );

        add_log(log);

        client_id++;

        if(server_state.connected_clients > 5)
        {
            server_state.connected_clients = 2;
        }
    }

    return NULL;
}

void show_popup(char* title, char* message)
{
    newtWinMessage(
        title,
        "OK",
        message
    );
}

void draw_dashboard()
{
    char status[512];
    char logs[8192] = "";

    snprintf(
        status,
        sizeof(status),
        "Clientes conectados: %d\n\n"
        "Total de votos: %d\n\n"
        "Threads ativas: %d\n",
        server_state.connected_clients,
        server_state.total_votes,
        server_state.active_threads
    );

    for(int i = 0; i < server_state.log_count; i++)
    {
        strcat(logs, server_state.logs[i]);
        strcat(logs, "\n");
    }

    newtComponent form;
    newtComponent lbl_title;
    newtComponent status_box;
    newtComponent logs_box;
    newtComponent btn_reports;
    newtComponent btn_logs;
    newtComponent btn_clients;
    newtComponent btn_exit;

    // Ajustado para 76 colunas por 22 linhas (Cabe folgado no limite de 80x25)
    newtCenteredWindow(76, 22, "Servidor de Votacao");

    lbl_title = newtLabel(2, 1, "SERVIDOR ONLINE");

    // Reduzido ligeiramente a largura para 22
    status_box = newtTextbox(2, 3, 22, 12, NEWT_FLAG_BORDER);
    newtTextboxSetText(status_box, status);

    // Posicionado na coluna 26, com largura de 47 e altura de 14 para não estourar a janela
    logs_box = newtTextbox(26, 3, 47, 14, NEWT_FLAG_SCROLL | NEWT_FLAG_BORDER);
    newtTextboxSetText(logs_box, logs);

    // Botões reposicionados na linha 19 (O original na linha 24 sumia no terminal puro)
    btn_reports = newtButton(2, 19, "F1 Relat");
    btn_logs    = newtButton(17, 19, "F2 Logs");
    btn_clients = newtButton(30, 19, "F3 Clien");
    btn_exit    = newtButton(58, 19, "Q Sair"); // Movido mais para a esquerda para não cortar

    form = newtForm(NULL, NULL, 0);

    newtFormAddComponents(
        form, lbl_title, status_box, logs_box,
        btn_reports, btn_logs, btn_clients, btn_exit, NULL
    );

    newtComponent result = newtRunForm(form);

    if(result == btn_reports)
    {
        show_popup("Relatorios", "Modulo de relatorios em desenvolvimento");
    }
    else if(result == btn_logs)
    {
        show_popup("Logs", "Visualizacao detalhada de logs");
    }
    else if(result == btn_clients)
    {
        show_popup("Clientes", "Clientes conectados atualmente");
    }
    else if(result == btn_exit)
    {
        running = 0;
    }

    newtFormDestroy(form);
}
    char status[512];

    char logs[8192] = "";

    snprintf(
        status,
        sizeof(status),

        "Clientes conectados: %d\n\n"
        "Total de votos: %d\n\n"
        "Threads ativas: %d\n",

        server_state.connected_clients,
        server_state.total_votes,
        server_state.active_threads
    );

    for(int i = 0; i < server_state.log_count; i++)
    {
        strcat(logs, server_state.logs[i]);
        strcat(logs, "\n");
    }

    newtComponent form;

    newtComponent lbl_title;

    newtComponent status_box;

    newtComponent logs_box;

    newtComponent btn_reports;

    newtComponent btn_logs;

    newtComponent btn_clients;

    newtComponent btn_exit;

    newtCenteredWindow(
        90,
        28,
        "Servidor de Votacao"
    );

    lbl_title = newtLabel(
        2,
        1,
        "SERVIDOR ONLINE"
    );

    status_box = newtTextbox(
        2,
        3,
        25,
        12,
        NEWT_FLAG_BORDER
    );

    newtTextboxSetText(
        status_box,
        status
    );

    logs_box = newtTextbox(
        30,
        3,
        55,
        18,
        NEWT_FLAG_SCROLL | NEWT_FLAG_BORDER
    );

    newtTextboxSetText(
        logs_box,
        logs
    );

    btn_reports = newtButton(
        2,
        24,
        "F1 Relatorios"
    );

    btn_logs = newtButton(
        20,
        24,
        "F2 Logs"
    );

    btn_clients = newtButton(
        35,
        24,
        "F3 Clientes"
    );

    btn_exit = newtButton(
        70,
        24,
        "Q Sair"
    );

    form = newtForm(NULL, NULL, 0);

    newtFormAddComponents(
        form,

        lbl_title,

        status_box,

        logs_box,

        btn_reports,

        btn_logs,

        btn_clients,

        btn_exit,

        NULL
    );

    newtComponent result = newtRunForm(form);

    if(result == btn_reports)
    {
        show_popup(
            "Relatorios",
            "Modulo de relatorios em desenvolvimento"
        );
    }

    else if(result == btn_logs)
    {
        show_popup(
            "Logs",
            "Visualizacao detalhada de logs"
        );
    }

    else if(result == btn_clients)
    {
        show_popup(
            "Clientes",
            "Clientes conectados atualmente"
        );
    }

    else if(result == btn_exit)
    {
        running = 0;
    }

    newtFormDestroy(form);
}

void start_dashboard()
{
    add_log("[INFO] Servidor iniciado");
    add_log("[INFO] Interface carregada");
    add_log("[INFO] Sistema pronto");

    pthread_t sim_thread;

    pthread_create(
        &sim_thread,
        NULL,
        simulation_thread,
        NULL
    );
    
    pthread_t network_thread;

	pthread_create(
		&network_thread,
		NULL,
		start_server,
		NULL
	);

    while(running)
    {
        draw_dashboard();

        usleep(100000);
    }

    pthread_join(sim_thread, NULL);
}
