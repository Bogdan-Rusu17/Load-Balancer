/* Copyright 2023 <> */
#ifndef SERVER_H_
#define SERVER_H_

#include "hashtable.h"
#define KEY_LENGTH 128
#define VALUE_LENGTH 65536

typedef struct pair_info {
    char key[KEY_LENGTH];
    char value[VALUE_LENGTH];
} pair_info;

struct server_memory;
typedef struct server_memory server_memory;

struct server_memory {
	hashtable_t *ht;
};

server_memory *init_server_memory();

void free_server_memory(server_memory *server);

void server_store(server_memory *server, char *key, char *value);

void server_remove(server_memory *server, char *key);

char *server_retrieve(server_memory *server, char *key);

void server_ht_contents_free(server_memory *server);

#endif /* SERVER_H_ */
