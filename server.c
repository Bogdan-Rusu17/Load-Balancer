/*COPYRIGHT RUSU BOGDAN, 312CA 2022-2023*/

#include <stdlib.h>
#include <string.h>

#include "./server.h"
#include "./utils.h"

/*
	function that initializes the server memory, i.e.
	allocates the memory for a server_memory instance
	that contains a pointer to a hashtable
*/

server_memory *init_server_memory(void)
{
	server_memory *server = malloc(sizeof(server_memory));
	DIE(server == NULL, "error allocating server\n");
	server->ht = ht_create(HMAX, compare_function_strings, hash_function_string);
	DIE(server->ht == NULL, "error allocating ht for server\n");
	return server;
}

/*
	function that puts the argument key into the hashtable member
	of the corresponding server
*/

void server_store(server_memory *server, char *key, char *value) {
	ht_put(server->ht, key, KEY_LENGTH, value, VALUE_LENGTH);
}

/*
	function that retrieves the value of 
	the argument key from the hashtable member
	of the corresponding server
*/

char *server_retrieve(server_memory *server, char *key) {
	return ht_get(server->ht, key);
}

/*
	function that removes the pair corresponding to
	the argument key from the hashtable member
	of the corresponding server
*/

void server_remove(server_memory *server, char *key) {
	ht_remove_entry(server->ht, key);
}

/*
	function that frees the memory for the hashtable member
	of the corresponding server and for the server itself
*/

void free_server_memory(server_memory *server) {
	ht_free(server->ht);
	free(server);
}

