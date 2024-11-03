#include "kvs.h"

// char* get(kvs_t* kvs, const char* key)
// {
// 	/* do program here */
// 	//if key not found : return -1
// 	char* value = (char*)malloc(sizeof(char)*100);

// 	if(!value){
// 		printf("Failed to malloc\n");
// 		return NULL;
// 	}

// 	strcpy(value, "deadbeaf");
// 	return value;

// }

char* get(kvs_t* kvs, const char* key) {
    node_t* node = kvs->header;
    for (int i = kvs->kvs_mx_level; i >= 0; i--) {
        while (node->next[i] && strcmp(node->next[i]->key, key) < 0) {
            node = node->next[i];
        }
    }
    node = node->next[0];

    if (node && strcmp(node->key, key) == 0) {
        return node->value;
    }
    return NULL;
}
