CC = gcc  # C 컴파일러로 변경
CFLAGS = -Wall  # C에서 사용할 컴파일 플래그 (예: 경고 메시지 활성화)

all: dec2hex.exe decfp2hex.exe

dec2hex.exe: dec2hex.o
	$(CC) -o $@ $^ -std=c11 $(CFLAGS) $(LDFLAGS)

decfp2hex.exe: decfp2hex.o
	$(CC) -o $@ $^ -std=c11 $(CFLAGS) $(LDFLAGS)

%.o: %.c $(DEPS_CHAT)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f *.exe
	rm -f *.o *~ *.stackdump
