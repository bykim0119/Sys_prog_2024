#include "kvs.h"

int rand_lv(){ //레벨 난수 생성 함수
	srand(time(NULL));
    int kvs_mx_level = 0;
    while (rand() < RAND_MAX / 2 && kvs_mx_level < MAX_LEVEL - 1) {
        kvs_mx_level++;
    }
    return kvs_mx_level;
}

kvs_t* open() {

    kvs_t* kvs = (kvs_t*) malloc (sizeof(kvs_t));
	if (!kvs) {
		printf("Allocating kvs Failed\n");
		return NULL;
	}
    kvs->kvs_mx_level = 0;
    kvs->items = 0;
    
    kvs->header = (node_t*)malloc(sizeof(node_t));

    if (!kvs->header) {
        printf("Allocating header failed\n");
        free(kvs);
        return NULL;
    }
    for (int i = 0; i < MAX_LEVEL; i++) {
        kvs->header->next[i] = NULL;
    }

	
    FILE* snapshotFile = fopen("kvs.img", "r");
    if (snapshotFile) {
        printf("now kvs recovering from image file . . .\n");
        if(do_recovery(kvs, snapshotFile)){
            printf("kvs recovery success!\n");
        };
        fclose(snapshotFile);
        printf("Open: kvs has %d items\n", kvs->items);
        return kvs;
    }

    printf("Open: kvs has %d items\n", kvs->items);
    return kvs;
}

int do_recovery(kvs_t* kvs, FILE* file) {
    if (!file || !kvs) return 0;

    // 메타데이터 읽기
    if (fscanf(file, "%d,%d\n", &kvs->kvs_mx_level, &kvs->items) != 2) {
        printf("Failed to read metadata from snapshot file\n");
        return 0;
    }

    // 데이터 읽기
    char key[100];
    char* value = NULL;
    int node_level;
    node_t* prev_nodes[MAX_LEVEL];

    for (int i = 0; i < MAX_LEVEL; i++) {
        prev_nodes[i] = kvs->header;
    }

    // 각 노드 데이터 복원
    while (fscanf(file, "%99[^,],%m[^,],%d\n", key, &value, &node_level) == 3) {
        // 새로운 노드 생성
        node_t* new_node = (node_t*)malloc(sizeof(node_t));
        if (!new_node) {
            printf("Memory allocation failed for node\n");
            free(value);
            return 0;
        }

        // 키 복사 및 값 동적 할당
        strcpy(new_node->key, key);
        new_node->value = value; // fscanf에서 할당한 메모리를 사용

        for (int i = 0; i < MAX_LEVEL; i++) {
            new_node->next[i] = NULL;
        }

        // 레벨별로 링크 업데이트
        for (int i = 0; i < node_level; i++) {
            prev_nodes[i]->next[i] = new_node;
            prev_nodes[i] = new_node;
        }

        // 다음 값 초기화
        value = NULL; // fscanf가 새로 메모리를 할당하도록 초기화
    }

    return 1;
}

int do_snapshot(kvs_t* kvs) { //나이브한 snapshot
    FILE* imgFile = fopen("kvs.img", "w");

    if (!kvs || !imgFile) return 0;
    
    fprintf(imgFile, "%d,%d\n", kvs->kvs_mx_level, kvs->items); //메타데이터
    
    node_t* current = kvs->header->next[0]; // 레벨 0부터 시작
    while (current) {
        // 각 노드의 키, 값, 레벨을 저장
        int node_level = 0;
        for (int i = 0; i < MAX_LEVEL; i++) {
            if (current->next[i]) node_level++;
            else break;
        }
        fprintf(imgFile, "%s,%s,%d\n", current->key, current->value, node_level);
        current = current->next[0]; 
    }
    
	if (fflush(imgFile) != 0) {
		perror("fflush");
		fclose(imgFile);
		return 1;
	}
		
	int fd = fileno(imgFile);
	if (fd == -1) {
		perror("fileno");
		fclose(imgFile);
		return 1;
	}

	if (fsync(fd) != 0) {
		perror("fsync");
		fclose(imgFile);
	}
	printf("fsync success\n");
	fclose(imgFile);

    return 1;
}

