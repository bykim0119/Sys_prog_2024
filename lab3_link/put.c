#include "kvs.h"

int put(kvs_t* kvs, const char* key, const char* value) {
    node_t* update[MAX_LEVEL]; //MAX_LEVEL = 16
    node_t* node = kvs->header;

	printf("put: %s, %s\n", key, value);

    for (int i = kvs->kvs_mx_level; i >= 0; i--) {
        while (node->next[i] && strcmp(node->next[i]->key, key) < 0) {
            node = node->next[i];
        }
        update[i] = node;
    }
    node = node->next[0];

    if (node && strcmp(node->key, key) == 0) {
        free(node->value);
        node->value = strdup(value);
        return 0;
    } else {
        int level = rand_lv();
        if (level > kvs->kvs_mx_level) {
            for (int i = kvs->kvs_mx_level + 1; i <= level; i++) {
                update[i] = kvs->header;
            }
            kvs->kvs_mx_level = level;
        }

        node = (node_t*)malloc(sizeof(node_t));
        strcpy(node->key, key);
        node->value = strdup(value);

        for (int i = 0; i <= level; i++) {
            node->next[i] = update[i]->next[i];
            update[i]->next[i] = node;
        }
        kvs->items++;
        return 0;
    }
}
