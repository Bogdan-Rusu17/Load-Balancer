/* Copyright 2023 <> */
#ifndef LOAD_BALANCER_H_
#define LOAD_BALANCER_H_

#include "server.h"
#include "utils.h"
#define KEY_LENGTH 128
#define VALUE_LENGTH 65536
#define MAX_SERVERS 99999

struct hash_ring_elem;
typedef struct hash_ring_elem hash_ring_elem;

struct load_balancer;
typedef struct load_balancer load_balancer;

load_balancer *init_load_balancer();

void free_load_balancer(load_balancer *main);

void loader_store(load_balancer *main, char *key, char *value, int *server_id);

char *loader_retrieve(load_balancer *main, char *key, int *server_id);

void loader_add_server(load_balancer *main, int server_id);

void loader_remove_server(load_balancer *main, int server_id);

#endif /* LOAD_BALANCER_H_ */
