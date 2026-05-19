#include <stdio.h>
#include <string.h>

#include "server_state.h"

ServerState server_state = {
    .connected_clients = 0,
    .total_votes = 0,
    .active_threads = 1,
    .log_count = 0
};

void add_log(const char *message)
{
    if(server_state.log_count >= MAX_LOGS)
    {
        for(int i = 1; i < MAX_LOGS; i++)
        {
            strcpy(
                server_state.logs[i - 1],
                server_state.logs[i]
            );
        }

        server_state.log_count--;
    }

    snprintf(
        server_state.logs[server_state.log_count],
        256,
        "%s",
        message
    );

    server_state.log_count++;
}
