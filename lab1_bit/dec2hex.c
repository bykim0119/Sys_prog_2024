#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>

typedef unsigned char* pointer;

bool is64bit()
{
	return sizeof(long)==8?true:false; //64bit머신에서는 long타입이 64bit(8Byte)임.
}

bool isBigEndian(){
	unsigned int x = 0x0001;
  char *c = (char*)&x; //byte level reading
	if (c[0]){ //만약 c의 가장 첫부분부터 값이 저장되어 있다면 little endian. 아니면 big endian
		return false;
	}
	else
		return true;
}

void le_show_bytes(unsigned int* a, size_t sz)
{
	printf("%.8X\n", *a); //단순 8자리 16진법에 맞게 출력
	//char *c = (char*)&a; //함수 이름에 맞게 little endian으로 출력하고 싶을 때 사용
	//printf("0x");
	//for (size_t i = 0; i < sz; i++) {
  //  printf("%02X", c[i]);
  //}
	//printf("0xFFFFFFFF\n");
	return;
}

void be_show_bytes(unsigned int* a, size_t sz){
	printf("%.8X\n", *a); //단순 8자리 16진법에 맞게 출력
	//char *c = (char*)&a; //함수 이름에 맞게 big endian으로 출력하고 싶을 때 사용
  //printf("0x");
  //for (size_t i = 0; i < sz; i++) {
  //printf("%02X", c[i]);
  //}
	//printf("0xFFFFFFFF\n");
	return;
}

int main(int argc, char* argv[])
{
	if(argc < 2){
		printf("Usage: ./a.out number\n");
		exit(0);
	}

	unsigned int a = atoi(argv[1]);
	
	printf("ARCH=%d\n", is64bit()? 64 : 32);
	printf("ORDERING=%s\n", isBigEndian()? "BIG_ENDIAN": "LITTLE_ENDIAN");

	printf("MYANS: DEC=%d HEX=", a);
	isBigEndian()? be_show_bytes(&a, sizeof(unsigned int)): le_show_bytes(&a, sizeof(unsigned int));

	printf("CHECK: DEC=%d HEX=%.8X\n", a, a);
	return 0;
}
