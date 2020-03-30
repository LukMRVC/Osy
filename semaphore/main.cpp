//
// Created by lukas on 27.03.20.
//
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

int readline(int fd, char *line, int maxLen, int reset = 0) {
    static int initialRead = 1;
    static char buffer[255];
    static int readBytes = 0;
    static int readFromBuffer;
    int tempStrOffset = 0;
    int lineLen = 0;
    if (initialRead || reset) {
        readBytes = read(fd, buffer, 255);
        if (readBytes <= 0)
            return readBytes;
        initialRead = 0;
        readFromBuffer = 0;
    }
    if (readBytes == 0)
        return 0;
    int i = 0;

    while(buffer[readFromBuffer + i] != '\n' && i < (maxLen - 1)) {
        ++i;
        ++lineLen;
        if ( (readFromBuffer + i) == readBytes) {
            memcpy(line, buffer + readFromBuffer, i - 1);
            readBytes = read(fd, buffer, 255);
            if (readBytes == 0)
                break;
            else if (readBytes < 0) {
                lineLen = readBytes;
                break;
            }
            tempStrOffset = i;
            readFromBuffer = 0;
            i = 0;
        }
    }
    memcpy(line + tempStrOffset, buffer + readFromBuffer, i);
    line[tempStrOffset + i] = '\0';
    readFromBuffer += i + 1; // skip a newline
    return lineLen;
}


int main(void) {
    int fd = open("../lines.txt", O_RDONLY);
    char * line = (char*)malloc(sizeof(char) * 200);
    int ret = readline(fd, line, 200);
    while (readline(fd, line, 200) > 0) {
        printf("Line is: %s\n", line);
    }
    close(fd);
    fd = open("../lines.txt", O_RDONLY);
    readline(fd, line, 200, 1);
    printf("Line is: %s\n", line);
    while (readline(fd, line, 200) > 0) {
        printf("Line is: %s\n", line);
    }
    close(fd);
    return 0;
}