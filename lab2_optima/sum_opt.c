#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<limits.h>

#define MAX_NUM 10
//#define MAX_NUM 1000000000

typedef double data_t;

struct vec {
	int len;
	data_t *data;
};
typedef struct vec vec_t;

int get_vec_element(vec_t* v, int idx, data_t *val)
{
	assert(v);
	// 중복된 바운더리 체크 제거
	*val = v->data[idx];
	return 1;
}

static inline int vec_length(vec_t* v) //inline함수 사용
{
	assert(v);
	return v->len;
}

void combine(vec_t* v, data_t *dest)
{
	assert(dest);
	long i; //for loop최적화
	long length = vec_length(v); //for loop최적화 - 함수를 loop밖으로
	long lim = length-1;
	data_t x = 0; //함수의 로컬변수 선언 후 사용
	data_t *d = v->data;
	for(i = 0; i < lim; i+=2) //loop unrolling 사용
		x = x + (d[i] + d[i+1]); //함수 호출대신 포인터 연산 사용
	
	for(;i < length;i++)
		x = x + d[i];
	*dest = x;
}

void init(vec_t* v)
{
	assert(v);
	v->len = MAX_NUM;
	v->data = (data_t*) malloc(sizeof(data_t) * MAX_NUM);

	// init 
	for(int i = 0; i < MAX_NUM; i++)
		v->data[i] = i;
}

int main()
{
	printf("This is the optimized version .. \n");

	vec_t info; 
	data_t result; 
	// init 
	init(&info);

	// combine 
	combine(&info, &result);
	//printf("combined val = %d\n", result);

	return 0;
}
