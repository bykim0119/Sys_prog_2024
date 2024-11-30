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

int bWrite(int fd, char* buffer, size_t* buffer_offset, const void* data, size_t size) { //buffer write, 버퍼 크기는 4MB
    if (*buffer_offset + size > SNAPSHOT_BUF_SIZE) { //버퍼가 꽉 차면 write하고 버퍼를 비운다.
        if (write(fd, buffer, *buffer_offset) != (ssize_t)*buffer_offset) {
            perror("Failed to flush buffer");
            return -1;
        }
        *buffer_offset = 0; 
    }

    memcpy(buffer + *buffer_offset, data, size);
    *buffer_offset += size;

    return 0; 
}


ssize_t bRead(int fd, char* buffer, ssize_t* buffer_size, ssize_t* buffer_offset, void* dest, size_t size) { //buffer read
    if (*buffer_offset + size > *buffer_size) { //buffer 크기 이상을 읽으려 할 때

        memmove(buffer, buffer + *buffer_offset, *buffer_size - *buffer_offset); //이미 bRead한 부분은 버리고 나머지를 버퍼 앞으로 옮긴다.
        *buffer_size -= *buffer_offset;
        *buffer_offset = 0;

        ssize_t bytes_read = read(fd, buffer + *buffer_size, SNAPSHOT_BUF_SIZE - *buffer_size); //남은 버퍼 크기를 뺀 만큼만 새로 읽어 온다.
        if (bytes_read <= 0) {
            return bytes_read; 
        }
        *buffer_size += bytes_read;
    }

    memcpy(dest, buffer + *buffer_offset, size); //dest에 버퍼 내용을 size만큼 적어준다
    *buffer_offset += size;
    return size;
}

int do_recovery(kvs_t* kvs, int fd) {
    if (!kvs || fd < 0) {
        return 0;
    }

    char buffer[SNAPSHOT_BUF_SIZE];
    ssize_t buffer_size = 0, buffer_offset = 0;


    if (bRead(fd, buffer, &buffer_size, &buffer_offset, &(kvs->items), sizeof(int)) != sizeof(int) || //kvs메타 데이터 읽기
        bRead(fd, buffer, &buffer_size, &buffer_offset, &(kvs->kvs_mx_level), sizeof(int)) != sizeof(int)) {
        perror("Failed to read kvs metadata");
        return -1;
    }


    size_t key_len, value_len;
    char key[100];
    char* value = NULL;


    node_t* prev_nodes[MAX_LEVEL];
    for (int i = 0; i < MAX_LEVEL; i++) {
        prev_nodes[i] = kvs->header;
    }
    while (bRead(fd, buffer, &buffer_size, &buffer_offset, &key_len, sizeof(size_t)) == sizeof(size_t)) {
        
        if (key_len > sizeof(key)) {
            printf("Key length exceeds buffer size\n");
            return -1;
        }

        if (bRead(fd, buffer, &buffer_size, &buffer_offset, key, key_len) != (ssize_t)key_len ||
            bRead(fd, buffer, &buffer_size, &buffer_offset, &value_len, sizeof(size_t)) != sizeof(size_t)) {
            perror("Failed to read kvs data");
            return -1;
        }

        value = (char*)malloc(value_len); //value_len길이만큼 value를 버퍼로부터 읽어 온다
        if (!value || bRead(fd, buffer, &buffer_size, &buffer_offset, value, value_len) != (ssize_t)value_len) {
            perror("Failed to allocate or read value");
            free(value);
            return -1;
        }

        // 여기부터 노드를 복원하는 내용 (baseline과 동일하다)
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

        int node_level = rand_lv(); 
        for (int i = 0; i <= node_level; i++) {
            new_node->next[i] = prev_nodes[i]->next[i];
            prev_nodes[i]->next[i] = new_node;
            prev_nodes[i] = new_node;
        }

        value = NULL;
    }
    return 1;
}


int do_snapshot(kvs_t* kvs) { //텍스트 기반 파일 -> 바이너리 기반 파일 생성으로 최적화.
    if (!kvs || !kvs->header) {
        return -1;
    }

    int fd = open("kvs.img", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Failed to open snapshot file");
        return -1;
    }

    char buffer[SNAPSHOT_BUF_SIZE]; //버퍼 사이즈를 크게 늘렸다(fprintf는 버퍼가 4KB나 8KB로 작음) -> 보통 이미지 파일은 크니까 버퍼의 크기를 키워 최적화
    size_t buffer_offset = 0; //버퍼에 얼마나 할당되었는 지 확인할 수 있는 오프셋


    if (bWrite(fd, buffer, &buffer_offset, &(kvs->items), sizeof(int)) < 0 || //메타데이터 저장(최대 레벨, 아이템수)
        bWrite(fd, buffer, &buffer_offset, &(kvs->kvs_mx_level), sizeof(int)) < 0) {
        close(fd);
        return -1;
    }


    node_t* node = kvs->header->next[0];
    while (node) {
        size_t key_len = strlen(node->key) + 1; 
        size_t value_len = strlen(node->value) + 1;

        if (bWrite(fd, buffer, &buffer_offset, &key_len, sizeof(size_t)) < 0 || //키 길이, 키, value길이, value 형태로 저장하여 바이너리 파일을 순차적으로 읽을 수 있음
            bWrite(fd, buffer, &buffer_offset, node->key, key_len) < 0 ||       //길이 정보 저장으로 인해 기존 이미지 파일보다 커질 수 있음
            bWrite(fd, buffer, &buffer_offset, &value_len, sizeof(size_t)) < 0 ||
            bWrite(fd, buffer, &buffer_offset, node->value, value_len) < 0) {
            close(fd);
            return -1;
        }

        node = node->next[0];
    }

    
    if (buffer_offset > 0) { //버퍼가 덜 찬 상태에서 종료되면 그 내용도 write할 수 있게 하기 위함.
        if (write(fd, buffer, buffer_offset) != (ssize_t)buffer_offset) {
            perror("Failed to flush final buffer");
            close(fd);
            return -1;
        }
    }

    if (fsync(fd) < 0) { //fsync
    perror("Failed to fsync");
    close(fd);
    return -1;
}

    close(fd);
    return 1;
}


