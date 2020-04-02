//
// Created by Lukas on 30.03.2020.
//

#include <stdio.h>
#include "readline.cpp"

int main(void) {

    char line[512];

    while (readline(0, line, 512) > 0) {
        printf("%s\n", line);
        fflush(stdout);
    }
    return 0;
}
