#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

extern sqlite3 *db;

int init_database();

int voter_exists(const char *voter_id);

int has_voted(const char *voter_id);

int count_votes();

int save_vote(
    const char *voter_id,
    const char *candidate,
    const char *receipt
);

#endif
