#ifndef SERVER_STATE_H
#define SERVER_STATE_H

#define MAX_LOGS 200

typedef struct
{
    int connected_clients;
    int total_votes;
    int active_threads;

    char logs[MAX_LOGS][256];
    int log_count;

} ServerState;

extern ServerState server_state;

void add_log(const char *message);

#endif
