/*COPYRIGHT RUSU BOGDAN, 312CA 2022-2023*/
#include "./linked_list.h"

linked_list_t*
ll_create(unsigned int data_size)
{
    linked_list_t* list = malloc(sizeof(*list));
    list->head = NULL;
    list->data_size = data_size;
    list->size = 0;
    return list;
}

void ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data)
{
    if (list == NULL) {
        return;
    }
    if (n > list->size)
        n = list->size;
    ll_node_t *new_node = malloc(sizeof(*new_node));
    new_node->data = malloc(list->data_size);
    memcpy(new_node->data, new_data, list->data_size);
    ll_node_t *curr, *prev;
    prev = NULL;
    curr = list->head;
    while (n) {
        n--;
        prev = curr;
        curr = curr->next;
    }

    if (prev == NULL)
        list->head = new_node;
    else
        prev->next = new_node;
    new_node->next = curr;
    list->size++;
}

ll_node_t* ll_remove_nth_node(linked_list_t* list, unsigned int n)
{
    if (list == NULL)
        return NULL;
    if (n >= list->size - 1)
        n = list->size - 1;
    ll_node_t *prev, *curr;
    prev = NULL;
    curr = list->head;
    while (n) {
        n--;
        prev = curr;
        curr = curr->next;
    }
    if (prev == NULL) {
        list->head = curr->next;
    } else {
        prev->next = curr->next;
    }
    list->size--;
    return curr;
}

unsigned int ll_get_size(linked_list_t* list)
{
    return list->data_size;
}

void ll_free(linked_list_t** pp_list)
{
    ll_node_t *curr, *to_del;
    linked_list_t *list = *pp_list;
    curr = list->head;
    while (curr) {
        to_del = curr;
        curr = curr->next;
        free(to_del->data);
        free(to_del);
    }
    free(list);
    list = NULL;
}

void ll_print_int(linked_list_t* list)
{
    if (list == NULL)
        return;
    ll_node_t *curr = list->head;
    while (curr != NULL) {
        printf("%d ", *((int *)curr->data));
        curr = curr->next;
    }
    printf("\n");
}

void ll_print_string(linked_list_t* list)
{
    if (list == NULL)
        return;
    ll_node_t *curr = list->head;
    while (curr != NULL) {
        printf("%s ", ((char *)curr->data));
        curr = curr->next;
    }
    printf("\n");
}
