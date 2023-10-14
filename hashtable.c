/*COPYRIGHT RUSU BOGDAN, 312CA 2022-2023*/
#include "./hashtable.h"

int compare_function_ints(void *a, void *b)
{
	int int_a = *((int *)a);
	int int_b = *((int *)b);

	if (int_a == int_b) {
		return 0;
	} else if (int_a < int_b) {
		return -1;
	} else {
		return 1;
	}
}

int compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}

/*
 * Functii de hashing:
 */
unsigned int hash_function_int(void *a)
{
	/*
	 * Credits: https://stackoverflow.com/a/12996028/7883884
	 */
	unsigned int uint_a = *((unsigned int *)a);

	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = (uint_a >> 16u) ^ uint_a;
	return uint_a;
}

unsigned int hash_function_string(void *a)
{
	/*
	 * Credits: http://www.cse.yorku.ca/~oz/hash.html
	 */
	unsigned char *puchar_a = (unsigned char*) a;
	uint64_t hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */

	return hash;
}

hashtable_t *ht_create(unsigned int hmax,
		int (*compare_function)(void*, void*),
		unsigned int (*hash_function)(void *))
{
	hashtable_t *ht = malloc(sizeof(hashtable_t));

	ht->hmax = hmax;
	ht->hash_function = hash_function;
	ht->size = 0;
	ht->compare_function = compare_function;
	ht->buckets = malloc(hmax * sizeof(linked_list_t *));
	DIE(ht->buckets == NULL, "arr of buckets alloc err\n");
	for (int i = 0; i < (int)hmax; i++) {
		ht->buckets[i] = ll_create(sizeof(info));
		DIE(ht->buckets[i] == NULL, "bucket alloc err\n");
	}
	return ht;
}

int ht_has_key(hashtable_t *ht, void *key)
{
	if (ht == NULL)
		return 0;

	int bucket_ind = ht->hash_function(key) % ht->hmax;
	ll_node_t *curr = ht->buckets[bucket_ind]->head;
	while (curr) {
		if (ht->compare_function(((info *)curr->data)->key, key) == 0)
			return 1;
		curr = curr->next;
	}

	return 0;
}

void *ht_get(hashtable_t *ht, void *key)
{
	if (ht == NULL)
		return NULL;

	int bucket_ind = ht->hash_function(key) % ht->hmax;
	ll_node_t *curr = ht->buckets[bucket_ind]->head;
	while (curr) {
		if (ht->compare_function(((info *)curr->data)->key, key) == 0)
			return ((info *)curr->data)->value;
		curr = curr->next;
	}

	return NULL;
}

void ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
	if (ht == NULL)
		return;
	int bucket_ind = ht->hash_function(key) % ht->hmax;
	ll_node_t *curr, *prev = NULL;
	curr = ht->buckets[bucket_ind]->head;
	while (curr) {
		prev = curr;
		curr = curr->next;
	}
	ll_node_t *new = malloc(sizeof(ll_node_t));
	new->data = malloc(sizeof(info));
	new->next = NULL;
	void *p = malloc(value_size);
	memcpy(p, value, value_size);
	void *q = malloc(key_size);
	memcpy(q, key, key_size);
	((info *)new->data)->key = q;
	((info *)new->data)->value = p;
	if (prev == NULL)
		ht->buckets[bucket_ind]->head = new;
	else
		prev->next = new;
	ht->buckets[bucket_ind]->size++;
	ht->size++;
}

void ht_remove_entry(hashtable_t *ht, void *key)
{
	if (ht_has_key(ht, key) == 0)
		return;

	int bucket_ind = ht->hash_function(key) % ht->hmax;
	ll_node_t *curr, *prev = NULL;
	curr = ht->buckets[bucket_ind]->head;
	while (curr) {
		if (ht->compare_function(key, ((info *)curr->data)->key) == 0) {
			if (prev == NULL) {
				ht->buckets[bucket_ind]->head = curr->next;
				free(((info *)curr->data)->key);
				free(((info *)curr->data)->value);
				free(curr->data);
				free(curr);
				return;
			} else {
				prev->next = curr->next;
				free(((info *)curr->data)->key);
				free(((info *)curr->data)->value);
				free(curr->data);
				free(curr);
				return;
			}
		}
		prev = curr;
		curr = curr->next;
	}
}

void ht_free(hashtable_t *ht)
{
	for (int i = 0; i < (int)ht->hmax; i++) {
		ll_node_t *curr = ht->buckets[i]->head, *toDel;
		while (curr) {
			free(((info *)curr->data)->key);
			free(((info *)curr->data)->value);
			free(curr->data);
			toDel = curr;
			curr = curr->next;
			free(toDel);
		}
		free(ht->buckets[i]);
	}
	free(ht->buckets);
	free(ht);
}

unsigned int ht_get_size(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->size;
}

unsigned int ht_get_hmax(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->hmax;
}
