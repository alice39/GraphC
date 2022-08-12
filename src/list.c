#include <stdlib.h>

#include <list.h>

void list_init(struct list* list, lfree_t destroyer) {
    if (list == NULL) {
        return;
    }
    
    list->destroyer = destroyer;
    list->head = NULL;
    list->last = NULL;
}

bool list_empty(struct list* list) {
    return list == NULL || list->head == NULL;
}

void list_destroy(struct list* list) {
    if (list == NULL || list->head == NULL) {
        return;
    }

    struct list_node* previous_node = list->head;
    struct list_node* actual_node = list->head->next;

    for (; actual_node != NULL; actual_node = actual_node->next) {
        if (list->destroyer != NULL) {
            list->destroyer(previous_node->data);
        }
        free(previous_node);

        previous_node = actual_node;
    }
    
    if (list->destroyer != NULL) {
        list->destroyer(previous_node->data);
    }
    free(previous_node);

    list->destroyer = NULL;
    list->head = NULL;
    list->last = NULL;
}

void list_add_head(struct list* list, void* data) {
    if (list == NULL) {
        return;
    }

    struct list_node* new_node = malloc(sizeof(struct list_node));
    new_node->data = data;
    new_node->next = list->head;
    new_node->back = NULL;

    if (list->head == NULL) {
        list->head = new_node;
        list->last = new_node;
        return;
    }

    list->head->back = new_node;
    list->head = new_node;
}

void list_add_last(struct list* list, void* data) {
    if (list == NULL) {
        return;
    }

    struct list_node* new_node = malloc(sizeof(struct list_node));
    new_node->data = data;
    new_node->next = NULL;
    new_node->back = list->last;

    if (list->last == NULL) {
        list->head = new_node;
        list->last = new_node;
        return;
    }

    list->last->next = new_node;
    list->last = new_node;
}

void* list_del_head(struct list* list) {
    if (list == NULL || list->head == NULL) {
        return NULL;
    }
    
    void* data = list->head->data;

    if (list->head == list->last) {
        free(list->head);
        list->head = NULL;
        list->last = NULL;
    } else {
        struct list_node* new_head = list->head->next;
        free(list->head);

        new_head->back = NULL;
        list->head = new_head;
    }

    return data;
}

void* list_del_last(struct list* list) {
    if (list == NULL || list->last == NULL) {
        return NULL;
    }
    
    void* data = list->last->data;

    if (list->head == list->last) {
        free(list->last);
        list->head = NULL;
        list->last = NULL;
    } else {
        struct list_node* new_last = list->last->next;
        free(list->last);

        new_last->next = NULL;
        list->last = new_last;
    }

    return data;
}

