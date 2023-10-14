/*COPYRIGHT RUSU BOGDAN, 312CA 2022-2023*/
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "./load_balancer.h"

typedef struct hash_ring_elem {
	unsigned int val;
	int idx;
} hash_ring_elem;

struct load_balancer {
	server_memory **servers;
	int n_servers;
	hash_ring_elem *hash_ring;
};

unsigned int hash_function_servers(void *a) {
	unsigned int uint_a = *((unsigned int *)a);

	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = (uint_a >> 16u) ^ uint_a;
	return uint_a;
}

unsigned int hash_function_key(void *a) {
	unsigned char *puchar_a = (unsigned char *)a;
	unsigned int hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c;

	return hash;
}

/*
	function that computes the consistent hashing of the given key
	in the hash_ring, i.e. returns the id of the server which this key
	should belong to, through the use of binary search on the sorted
	array of replicas
*/

unsigned int consistent_hash(load_balancer *ldb, void *a)
{
	unsigned int uint_a = hash_function_key(a);
	int left = 0, mid, right = ldb->n_servers - 1;
	int sol = -1;
	mid = (left + right) >> 1;
	while (left <= right) {
		mid = (left + right) >> 1;
		if (ldb->hash_ring[mid].val > uint_a) {
			sol = ldb->hash_ring[mid].idx;
			right = mid - 1;
		} else if (ldb->hash_ring[mid].val == uint_a) {
			return mid;
		} else {
			left = mid + 1;
		}
	}

	if (sol == -1)
		return ldb->hash_ring[0].idx;
	return sol;
}

/*
	function that initializes the load_balancer needed for this task
	and its members
*/

load_balancer *init_load_balancer()
{
	load_balancer *ldb = malloc(sizeof(load_balancer));
	DIE(ldb == NULL, "loadbalancer alloc err\n");
	ldb->n_servers = 0;
	ldb->servers = malloc(MAX_SERVERS * sizeof(server_memory *));
	DIE(ldb->servers == NULL, "servers alloc err\n");
	for (int i = 0; i < MAX_SERVERS; i++)
		ldb->servers[i] = NULL;
	return ldb;
}

/*
	function that compares two hash_ring elements for the sake of
	qsorting the hash_ring
*/

int cmp_hash_ring(const void *a, const void *b)
{
	hash_ring_elem hash_ring_a = *(hash_ring_elem *)a;
	hash_ring_elem hash_ring_b = *(hash_ring_elem *)b;

	if (hash_ring_a.val < hash_ring_b.val)
		return -1;
	if (hash_ring_a.val == hash_ring_b.val &&
	    hash_ring_a.idx < hash_ring_b.idx)
		return -1;
	return 1;
}

/*
	function that adds the replicas of the servers onto the hash_ring, 
	qsorts the hashring and then moves the pairs key, value around the
	servers so as to keep them as close when being hashed to a more
	accurate server in terms of closeness
*/

void loader_add_server(load_balancer *ldb, int server_id) {
	ldb->servers[server_id] = init_server_memory();
	ldb->n_servers += 3;
	int index;
	if (ldb->n_servers == 3) {
		ldb->hash_ring = malloc(3 * sizeof(hash_ring_elem));
		DIE(ldb->hash_ring == NULL, "error allocating\n");
		ldb->hash_ring[0].idx = server_id;
		ldb->hash_ring[1].idx = server_id;
		ldb->hash_ring[2].idx = server_id;
		index = server_id;
		ldb->hash_ring[0].val = hash_function_servers(&index);
		index += 100000;
		ldb->hash_ring[1].val = hash_function_servers(&index);
		index += 100000;
		ldb->hash_ring[2].val = hash_function_servers(&index);
	} else {
		hash_ring_elem *tmp = realloc(
			ldb->hash_ring,
			ldb->n_servers * sizeof(hash_ring_elem));

		DIE(tmp == NULL, "error allocating\n");
		ldb->hash_ring = tmp;
		index = ldb->n_servers / 3 - 1;
		int replica = server_id;
		ldb->hash_ring[index * 3].idx = server_id;
		ldb->hash_ring[index * 3 + 1].idx = server_id;
		ldb->hash_ring[index * 3 + 2].idx = server_id;
		ldb->hash_ring[index * 3].val = hash_function_servers(&replica);
		replica += 100000;
		ldb->hash_ring[index * 3 + 1].val = hash_function_servers(&replica);
		replica += 100000;
		ldb->hash_ring[index * 3 + 2].val = hash_function_servers(&replica);
	}
	qsort(ldb->hash_ring, ldb->n_servers, sizeof(hash_ring_elem), cmp_hash_ring);

	if (ldb->n_servers > 3) {
		for (int i = 0; i < ldb->n_servers; i++) {
			if (ldb->hash_ring[i].idx == server_id) {
				int to_mov_serv_idx = i + 1;
				while (to_mov_serv_idx < ldb->n_servers - 1
				       && ldb->hash_ring[to_mov_serv_idx].idx == server_id)
					to_mov_serv_idx++;
				if (i == ldb->n_servers - 1)
					to_mov_serv_idx = 0;
				int extract_idx = ldb->hash_ring[to_mov_serv_idx].idx;

				for (int j = 0; j < HMAX; j++) {
					ll_node_t *to_mov = ldb->servers[extract_idx]->ht->buckets[j]->head;
					ll_node_t *next_node;
					while (to_mov) {
						next_node = to_mov->next;
						void *key = ((info *)to_mov->data)->key;
						void *value = ((info *)to_mov->data)->value;

						int position = consistent_hash(ldb, ((info *)to_mov->data)->key);
						if (position != ldb->hash_ring[to_mov_serv_idx].idx) {
							server_store(ldb->servers[position], key, value);
							server_remove(ldb->servers[extract_idx], key);
						}
						to_mov = next_node;
					}
				}
			}
		}
	}
}

/*
	function that removes the replicas of the server to be removed
	from the hashring, sorts it back in place and reassignes each
	pair consisting of key and value to the new server in which they
	will be stored through the use of the consistent hashing
*/

void loader_remove_server(load_balancer *ldb, int server_id) {
	for (int i = 0; i < ldb->n_servers; i++) {
		if (ldb->hash_ring[i].idx == server_id) {
			ldb->hash_ring[i].val = UINT_MAX;
		}
	}

	qsort(ldb->hash_ring, ldb->n_servers, sizeof(hash_ring_elem), cmp_hash_ring);
	ldb->n_servers -= 3;

	hash_ring_elem *tmp = realloc(
					ldb->hash_ring,
					ldb->n_servers * sizeof(hash_ring_elem));
	DIE(tmp == NULL, "error reallocating\n");

	ldb->hash_ring = tmp;


	for (int i = 0; i < (int)ldb->servers[server_id]->ht->hmax; i++) {
		ll_node_t *to_mov = ldb->servers[server_id]->ht->buckets[i]->head;
		while (to_mov) {
			void *value = ((info *)to_mov->data)->value;
			void *key = ((info *)to_mov->data)->key;
			int position = consistent_hash(ldb, key);
			server_store(ldb->servers[position], key, value);

			to_mov = to_mov->next;
		}
	}

	free_server_memory(ldb->servers[server_id]);
	ldb->servers[server_id] = NULL;
}

/*
	function that stores the given key into its corresponding server based
	on its consistent hash
*/

void loader_store(load_balancer *ldb, char *key, char *value, int *server_id) {
	int position = consistent_hash(ldb, key);
	*server_id = position;
	server_store(ldb->servers[position], key, value);
}

/*
	function that retrieves the given key into its corresponding server based
	on its consistent hash
*/

char *loader_retrieve(load_balancer *ldb, char *key, int *server_id) {
	int position = consistent_hash(ldb, key);
	*server_id = position;
	return server_retrieve(ldb->servers[position], key);
}

/*
	function that frees all the memory occupied by the load_balancer
	and all its contained servers
*/

void free_load_balancer(load_balancer *ldb) {
	for (int i = 0; i < MAX_SERVERS; i++) {
		if (ldb->servers[i]) {
			free_server_memory(ldb->servers[i]);
		}
	}
	free(ldb->servers);
	free(ldb->hash_ring);
	free(ldb);
}
