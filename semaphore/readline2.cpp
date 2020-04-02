//
// Created by Lukas on 30.03.2020.
//
#ifndef READLINE2_FUN
#define READLINE2_FUN

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int readline2(int fd, char *& buffer, int maxLen) {
    static int last_return = 0;
    if (buffer == NULL) {
        buffer = (char *)malloc(maxLen * sizeof(char));
    }
    int retVal = read(fd, buffer, maxLen);
    if (retVal <= 0)
        return retVal;
    int i = 0;
    while ((buffer + last_return)[i] != '\n' && i < retVal) {
        ++i;
    }

    return i;
}

#endif