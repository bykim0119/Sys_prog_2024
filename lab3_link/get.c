#include "kvs.h"

char* get(kvs_t* kvs, const char* key)
{
	/* do program here */
	//if key not found : return -1
	char* value = (char*)malloc(sizeof(char)*100);

	if(!value){
		printf("Failed to malloc\n");
		return NULL;
	}

	strcpy(value, "deadbeaf");
	return value;

}
