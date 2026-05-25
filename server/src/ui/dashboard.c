#include <newt.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#include "../database/database.h"
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

void update_dashboard_text(
    newtComponent status_box,
    newtComponent logs_box
)
{
    char status[512];
    char logs[8192] = "";
    int total_votes = count_votes();

    snprintf(
        status,
        sizeof(status),
        "Clientes: %d\n\n"
        "Votos: %d\n\n"
        "Threads: %d\n",
        server_state.connected_clients,
        total_votes,
        server_state.active_threads
    );

    for(int i = 0; i < server_state.log_count; i++)
    {
        strcat(logs, server_state.logs[i]);
        strcat(logs, "\n\n");
    }

    newtTextboxSetText(
        status_box,
        status
    );

    newtTextboxSetText(
        logs_box,
        logs
    );

    newtRefresh();
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

    newtComponent form;
    newtComponent lbl_title;
    newtComponent status_box;
    newtComponent logs_box;
    newtComponent btn_reports;
    newtComponent btn_logs;
    newtComponent btn_clients;
    newtComponent btn_exit;

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

    status_box = newtTextbox(
        2,
        3,
        18,
        12,
        NEWT_FLAG_BORDER
    );

    logs_box = newtTextbox(
        22,
        3,
        42,
        14,
        NEWT_FLAG_SCROLL | NEWT_FLAG_BORDER
    );

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

    while(running)
    {
        struct newtExitStruct exit_status;

        update_dashboard_text(
            status_box,
            logs_box
        );

        newtFormSetTimer(
            form,
            1000
        );

        newtFormRun(
            form,
            &exit_status
        );

        if(exit_status.reason == NEWT_EXIT_COMPONENT)
        {
            if(exit_status.u.co == btn_reports)
            {
                show_popup(
                    "Relatorios",
                    "Modulo de relatorios em desenvolvimento"
                );
            }
            else if(exit_status.u.co == btn_logs)
            {
                show_popup(
                    "Logs",
                    "Visualizacao detalhada de logs"
                );
            }
            else if(exit_status.u.co == btn_clients)
            {
                show_popup(
                    "Clientes",
                    "Clientes conectados atualmente"
                );
            }
            else if(exit_status.u.co == btn_exit)
            {
                running = 0;
            }
        }
    }

    newtFormDestroy(form);
}
