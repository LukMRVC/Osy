//
// Created by lukas on 26.03.20.
//
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>

int readline(int fd, char *line, int max_len) {
    static char buffer[255];
    static char tmpBuf[255];
    static int readBytes = 0;
    static int readFromBuffer = 0;
    if (readBytes - readFromBuffer == 0) {
        readBytes = read(fd, line, max_len);
        if (readBytes <= 0)
            return readBytes;
    }
    int i = readFromBuffer;
    for( ; buffer[i] != '\n' && i < readBytes; ++i);
    if (i >= readBytes && buffer[i] != '\n') { //precti dalsi data do bufferu
        memcpy(tmpBuf, buffer, i); //load buffer again and reset readFromBuffer and readBytes
    }
    ++i;
    memcpy(line, buffer + readFromBuffer, i);
    readFromBuffer += i;
    return i;
}


int main(void) {y
    int fd = open("/home/lukas/Documents/osy/socket/lines.txt", O_RDONLY);
    char * line = (char*) malloc(sizeof(char) * 200);
    readline(fd, line, 200);
    printf("Line is: %s", line);
    readline(fd, line, 200);
    printf("Line is: %s", line);
    readline(fd, line, 200);
    printf("Line is: %s", line);

    close(fd);
    printf("End of program");

    return 0;
}