#include "kvs.h"

// do_snapshot, do_recovery는 open.c에 정의
int main(int argc, char* argv[])
{
	kvs_t* kvs = open();

	if(!kvs) {
		printf("Failed to open kvs\n");
		return -1;
	}

	FILE* queryFile = fopen("../cluster004.trc", "r");
	
	if (!queryFile){
		printf("File not Found\n");
		kvs_close(kvs);
		return -1;
	}

	char op[10], key_q[100], *value_q = NULL; //buffer

    while (fscanf(queryFile, "%9[^,],%99[^,],%ms\n", op, key_q, &value_q) != EOF) {
		if (strcmp(op, "get") == 0){
			//set으로 찾은 결과는 kvs구성과 관련이 X
			//get으로 찾은 결과(value)를 answer.dat에 저장, 존재하지 않는 key면 -1로 저장.
			// char *return_val = get(kvs, key_q);
			// if (!return_val){
			// 	fprintf(answerFile, "%s\n", "-1");
			// 	continue;
			// }
			// fprintf(answerFile, "%s\n", return_val);
		}
		else if (strcmp(op, "set") == 0){
			//set(put)이면 새로운 key, value 등록
			put(kvs, key_q, value_q);
		}

		free(value_q);
		value_q = NULL;
    }
	
	if(!do_snapshot(kvs)) {
		printf("snapshot error: open file");
		return -1;
	}
		
	kvs_close(kvs);

	kvs_t* recovered_kvs = open(); //kvs.img를 통해 do_recovery로 kvs구성
	printf("key : %s, val : %s\n\n", "tweet55", get(recovered_kvs, "tweet55"));
	printf("key : %s, val : %s\n\n", "tweet13843", get(recovered_kvs, "tweet13843"));
	printf("key : %s, val : %s\n\n", "tweet3482", get(recovered_kvs, "tweet3482"));

	kvs_close(recovered_kvs);
	
	return 0;
}
