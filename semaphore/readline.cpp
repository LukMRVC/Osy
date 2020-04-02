//
// Created by Lukas on 30.03.2020.
//
#ifndef READLINE_FUN
#define READLINE_FUN
#include <unistd.h>
#include <string.h>

int readline(int fd, char *line, int maxLen, int reset = 0) {
    static int initialRead = 1;
    static const int bufferSize = 1024;
    static char buffer[bufferSize];
    static int readBytes = 0;
    static int readFromBuffer;
    int tempStrOffset = 0;
    int lineLen = 0;
    if (initialRead || reset) {
        readBytes = read(fd, buffer, bufferSize);
        if (readBytes <= 0)
            return readBytes;
//        printf("Buffer: %s", buffer);
        initialRead = 0;
        readFromBuffer = 0;
    }
    if (readBytes == 0)
        return 0;
    int i = 0;

    while(buffer[readFromBuffer + i] != '\n' && i < maxLen) {
        ++i;
        ++lineLen;
        if ( (readFromBuffer + i) >= readBytes) {
            memcpy(line, buffer + readFromBuffer, i - 1);
            readBytes = read(fd, buffer, bufferSize);
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
//    fflush(stdout);
    memcpy(line + tempStrOffset, buffer + readFromBuffer, i);
    line[tempStrOffset + i] = '\0';
    readFromBuffer += i + 1; // skip a newline
    if (readFromBuffer >= readBytes) {
        initialRead = 1;
    }
    return lineLen;
}

#endif