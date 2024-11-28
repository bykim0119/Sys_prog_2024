#include "kvs.h"

int rand_lv(){ //레벨 난수 생성 함수
	srand(time(NULL));
    int kvs_mx_level = 0;
    while (rand() < RAND_MAX / 2 && kvs_mx_level < MAX_LEVEL - 1) {
        kvs_mx_level++;
    }
    return kvs_mx_level;
}

kvs_t *kvs_open() {
    kvs_t *kvs = (kvs_t *)malloc(sizeof(kvs_t));
    if (!kvs) {
        printf("Allocating kvs Failed\n");
        return NULL;
    }

    kvs->kvs_mx_level = 0;
    kvs->items = 0;

    kvs->header = (node_t *)malloc(sizeof(node_t));
    if (!kvs->header) {
        printf("Allocating header failed\n");
        free(kvs);
        return NULL;
    }

    for (int i = 0; i < MAX_LEVEL; i++) {
        kvs->header->next[i] = NULL;
    }

    char* ftarget = "kvs.img";
    int fd = open(ftarget, O_RDONLY, 0644);
    printf("%d\n", fd);
    if (fd >= 0) {
        printf("now kvs recovering from image file . . .\n");
        if (do_recovery(kvs, fd)) {
            printf("kvs recovery success!\n");
        }
        close(fd);
        printf("Open: kvs has %d items\n", kvs->items);
        return kvs;
    }

    printf("Open: kvs has %d items\n", kvs->items);
    return kvs;
}

int do_recovery(kvs_t* kvs, int fd) {
    if (!kvs || fd < 0) {
        return 0;
    }

    // 기존 데이터 초기화
    node_t* current = kvs->header->next[0];
    while (current) {
        node_t* next = current->next[0];
        free(current->value);
        free(current);
        current = next;
    }
    for (int i = 0; i < MAX_LEVEL; i++) {
        kvs->header->next[i] = NULL;
    }
    kvs->items = 0;
    kvs->kvs_mx_level = 0;

    // kvs 메타데이터 복원
    if (read(fd, &(kvs->items), sizeof(int)) != sizeof(int) ||
        read(fd, &(kvs->kvs_mx_level), sizeof(int)) != sizeof(int)) {
        perror("Failed to read kvs metadata");
        return -1;
    }

    // 데이터 복원 및 노드 생성
    size_t key_len, value_len;
    char key[100];
    char* value = NULL;

    // 이전 레벨 노드들을 추적하기 위한 배열
    node_t* prev_nodes[MAX_LEVEL];
    for (int i = 0; i < MAX_LEVEL; i++) {
        prev_nodes[i] = kvs->header;
    }

    while (read(fd, &key_len, sizeof(size_t)) == sizeof(size_t)) {
        if (key_len > sizeof(key)) {
            fprintf(stderr, "Key length exceeds buffer size\n");
            return -1;
        }

        if (read(fd, key, key_len) != (ssize_t)key_len ||
            read(fd, &value_len, sizeof(size_t)) != sizeof(size_t)) {
            perror("Failed to read kvs data");
            return -1;
        }

        value = (char*)malloc(value_len);
        if (!value || read(fd, value, value_len) != (ssize_t)value_len) {
            perror("Failed to allocate or read value");
            free(value);
            return -1;
        }

        // 새로운 노드 생성
        node_t* new_node = (node_t*)malloc(sizeof(node_t));
        if (!new_node) {
            perror("Failed to allocate memory for new node");
            free(value);
            return -1;
        }

        strcpy(new_node->key, key);
        new_node->value = value;
        for (int i = 0; i < MAX_LEVEL; i++) {
            new_node->next[i] = NULL;
        }

        // 노드 연결 (레벨별로 연결)
        int node_level = rand_lv(); // 레벨 결정 (난수 생성)
        for (int i = 0; i <= node_level; i++) {
            new_node->next[i] = prev_nodes[i]->next[i];
            prev_nodes[i]->next[i] = new_node;
            prev_nodes[i] = new_node;
        }

        value = NULL;
    }

    return 1;
}



int do_snapshot(kvs_t* kvs) {
    if (!kvs || !kvs->header) {
        return -1;
    }

    int fd = open("kvs.img", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Failed to open snapshot file");
        return -1;
    }

    // kvs 메타데이터 저장
    if (write(fd, &(kvs->items), sizeof(int)) != sizeof(int) ||
        write(fd, &(kvs->kvs_mx_level), sizeof(int)) != sizeof(int)) {
        perror("Failed to write kvs metadata");
        close(fd);
        return -1;
    }

    // 데이터 저장
    node_t* current = kvs->header->next[0];
    while (current) {
        size_t key_len = strlen(current->key) + 1; // NULL 포함
        size_t value_len = strlen(current->value) + 1;

        if (write(fd, &key_len, sizeof(size_t)) != sizeof(size_t) ||
            write(fd, current->key, key_len) != (ssize_t)key_len ||
            write(fd, &value_len, sizeof(size_t)) != sizeof(size_t) ||
            write(fd, current->value, value_len) != (ssize_t)value_len) {
            perror("Failed to write kvs data");
            close(fd);
            return -1;
        }
        current = current->next[0];
    }

    close(fd);
    return 1;
}

//valgrind --tool=callgrind

// Snapshot 기능: KVS 상태를 kvs.img에 저장

// do_snapshot 함수#include <errno.h>

// do_snapshot: kvs 정보를 kvs.img에 저장
// int do_snapshot(kvs_t* kvs) {
//     if (!kvs || !kvs->header) {
//         return -1;
//     }

//     int fd = open("kvs.img", O_WRONLY | O_CREAT | O_TRUNC, 0644);
//     if (fd < 0) {
//         perror("Failed to open snapshot file");
//         return -1;
//     }

//     // kvs의 metadata 저장 (items, kvs_mx_level)
//     if (write(fd, &(kvs->items), sizeof(int)) != sizeof(int) ||
//         write(fd, &(kvs->kvs_mx_level), sizeof(int)) != sizeof(int)) {
//         perror("Failed to write kvs metadata");
//         close(fd);
//         return -1;
//     }

//     // 각 노드를 순회하며 key와 value 저장
//     node_t* current = kvs->header->next[0];
//     while (current) {
//         size_t key_len = strlen(current->key) + 1; // NULL 포함
//         size_t value_len = strlen(current->value) + 1;

//         if (write(fd, &key_len, sizeof(size_t)) != sizeof(size_t) ||
//             write(fd, current->key, key_len) != (ssize_t)key_len ||
//             write(fd, &value_len, sizeof(size_t)) != sizeof(size_t) ||
//             write(fd, current->value, value_len) != (ssize_t)value_len) {
//             perror("Failed to write kvs data");
//             close(fd);
//             return -1;
//         }
//         current = current->next[0];
//     }

//     close(fd);
//     return 1;
// }

// // do_recovery: kvs.img에서 데이터를 읽어 kvs 복원
// int do_recovery(kvs_t* kvs, int fd) {
//     if (!kvs || fd < 0) {
//         return 0;
//     }

//     // kvs의 metadata 복원
//     if (read(fd, &(kvs->items), sizeof(int)) != sizeof(int) ||
//         read(fd, &(kvs->kvs_mx_level), sizeof(int)) != sizeof(int)) {
//         perror("Failed to read kvs metadata");
//         return -1;
//     }

//     // 데이터 복원
//     size_t key_len, value_len;
//     char key[100];
//     char* value = NULL;

//     while (read(fd, &key_len, sizeof(size_t)) == sizeof(size_t)) {
//         if (key_len > sizeof(key)) {
//             fprintf(stderr, "Key length exceeds buffer size\n");
//             return -1;
//         }

//         if (read(fd, key, key_len) != (ssize_t)key_len ||
//             read(fd, &value_len, sizeof(size_t)) != sizeof(size_t)) {
//             perror("Failed to read kvs data");
//             return -1;
//         }

//         value = (char*)malloc(value_len);
//         if (!value || read(fd, value, value_len) != (ssize_t)value_len) {
//             perror("Failed to allocate or read value");
//             free(value);
//             return -1;
//         }

//         if (put(kvs, key, value) < 0) {
//             fprintf(stderr, "Failed to insert key-value during recovery\n");
//             free(value);
//             return -1;
//         }

//         free(value);
//         value = NULL;
//     }

//     return 1;
// }


