#include "kvs.h"

int rand_lv(){
	srand(time(NULL));
    int kvs_mx_level = 0;
    while (rand() < RAND_MAX / 2 && kvs_mx_level < MAX_LEVEL - 1) {
        kvs_mx_level++;
    }
    return kvs_mx_level;
}

kvs_t* open()
{
	kvs_t* kvs = (kvs_t*) malloc (sizeof(kvs_t));
	if (!kvs) {
		printf("Failed to allocate kvs\n");
		return NULL;
	}
    kvs->kvs_mx_level = 0;
    kvs->items = 0;
    
    kvs->header = (node_t*)malloc(sizeof(node_t));

    if (!kvs->header) {
        printf("Failed to allocate header\n");
        free(kvs);
        return NULL;
    }
    for (int i = 0; i < MAX_LEVEL; i++) {
        kvs->header->next[i] = NULL;
    }
	// if(kvs)
	// 	kvs->items = 0;
	printf("Open: kvs has %d items\n", kvs->items);


	return kvs;
}
