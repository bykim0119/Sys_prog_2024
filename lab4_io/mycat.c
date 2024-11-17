#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>

#include <unistd.h>
#include <fcntl.h>

#define BUFF_SIZE 16

int main(int argc, char* argv[]) {
    int ifd, ofd = STDOUT_FILENO; // 기본 출력은 stdout
    char* buff;
    ssize_t nreads, nwrites;

    if (argc < 2) {
        fprintf(stdout, "No arguments\n");
        return 0;
    } else if (argc == 4 && !strcmp(argv[2], "into")) {
        // 리다이렉션 설정
        if ((ofd = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1) {
            perror("Fail : open output file");
            return 1;
        }
    } else if (argc != 2) {
        fprintf(stdout, "Wrong arguments\n");
        return 0;
    }

    // Open input file
    if ((ifd = open(argv[1], O_RDONLY)) == -1) {
        perror("Fail : open input file");
        return 1;
    }

    // Allocate buffer
    if (!(buff = (char*) malloc(sizeof(char) * BUFF_SIZE))) {
        fprintf(stdout, "Fail : allocate memory\n");
        close(ifd);
        if (ofd != STDOUT_FILENO) close(ofd);
        return 1;
    }

    // Copy files
    while ((nreads = read(ifd, buff, BUFF_SIZE)) > 0) {
        nwrites = write(ofd, buff, nreads);
        if (nwrites == -1) {
            perror("Fail : write");
            break;
        }
    }
    if (nreads == -1) {
        perror("Fail : read");
    }

    // Close files 
    free(buff);
    close(ifd);
    if (ofd != STDOUT_FILENO) close(ofd);

    return 0;
}
