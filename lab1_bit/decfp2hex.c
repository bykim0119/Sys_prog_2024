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
  char *c = (char*)&x; //byte level reading(4Byte를 Byte단위로 읽음)
	if (c[0]){ //만약 c의 가장 첫부분부터 값이 저장되어 있다면 little endian. 아니면 big endian
		return false;
	}
	else
		return true;
}



void le_show_bytes(double* a, size_t sz)
{
	double fpart;
	long ipart = (long) *a; //int 부분과 소수점아래 부분을 parsing
	if (*a >= 0){
		fpart = *a - ipart;
		printf("%lX.", ipart);
	}
	else{
		fpart = -(*a - ipart);
		printf("-%lX.", -ipart);
	}
	
	while(fpart > 0){ //소수부분의 16진수 표현 구하기.
		fpart *= 16;
		int hpart = (int) fpart;
		printf("%X", hpart);
		fpart -= hpart;
	}
	printf("\n");
	return;
}

void be_show_bytes(double* a, size_t sz){
	double fpart;
	long ipart = (long) *a;
	if (*a >= 0){
		fpart = *a - ipart;
		printf("%lX.", ipart);
	}
	else{
		fpart = -(*a - ipart);
		printf("-%lX.", -ipart);
	}
	
	while(fpart > 0){
		fpart *= 16;
		int hpart = (int) fpart;
		printf("%X", hpart);
		fpart -= hpart;
	}
	printf("\n");
	return;
}

int main(int argc, char* argv[])
{
	if(argc < 2){
		printf("Usage: ./a.out number\n");
		exit(0);
	}

	double a = atof(argv[1]); //실수형태로 인자를 받음.
	
	printf("ARCH=%d\n", is64bit()? 64 : 32);
	printf("ORDERING=%s\n", isBigEndian()? "BIG_ENDIAN": "LITTLE_ENDIAN");

	printf("MYANS: DEC=%g HEX=", a);//소수점 부분 포매팅
	isBigEndian()? be_show_bytes(&a, sizeof(double)): le_show_bytes(&a, sizeof(double));

	return 0;
}
