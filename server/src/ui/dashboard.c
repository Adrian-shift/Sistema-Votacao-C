#include <newt.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "../models/server_state.h"
#include "../network/server_socket.h"
#include "dashboard.h"

static int running = 1;

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
        "Clientes: %d\n\n"
        "Votos: %d\n\n"
        "Threads: %d\n",
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

    // DEFINIÇÃO DA JANELA: Ajustada para 68x22 (cabe com sobra segura dentro de 73x27)
    newtCenteredWindow(
        68,
        22,
        "Servidor de Votacao"
    );

    lbl_title = newtLabel(
        2,
        1,
        "SERVIDOR ONLINE"
    );

    // CAIXA DE STATUS: Coluna 2, Largura 18, Altura 12
    status_box = newtTextbox(
        2,
        3,
        18,
        12,
        NEWT_FLAG_BORDER
    );

    newtTextboxSetText(
        status_box,
        status
    );

    // CAIXA DE LOGS: Empurrada para a Coluna 22, Largura 42 (22 + 42 = 64, respeitando o limite de 68)
    logs_box = newtTextbox(
        22,
        3,
        42,
        14,
        NEWT_FLAG_SCROLL | NEWT_FLAG_BORDER
    );

    newtTextboxSetText(
        logs_box,
        logs
    );

    // BOTÕES: Alinhados horizontalmente na linha 19 de forma compacta para não estourarem as laterais
    btn_reports = newtButton(
        2,
        19,
        "F1"
    );

    btn_logs = newtButton(
        14,
        19,
        "F2"
    );

    btn_clients = newtButton(
        26,
        19,
        "F3"
    );

    btn_exit = newtButton(
        50,
        19,
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
}
