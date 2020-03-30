//
// Created by lukas on 28.02.20.
//

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>
#include <sys/wait.h>

int eval(int a, char op, int b) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
        case '%': return a % b;
    }
}

int main() {
    srand(time(NULL));

    int pipes[2];
    int operandA;
    int operandB;
    char _operator;

    pipe(pipes);
    char operands[] = { '+', '-', '*', '/', '%' };

    if (fork() == 0) { //child
        for (int i = 0; i < 20; ++i) {
            close(pipes[0]);
            char buf[40];
            sprintf(buf, "%d%c%d", rand() % 100, operands[rand() % 5], (rand() % 100) + 1);
            usleep(1000);
            write(pipes[1], buf, strlen(buf));
        }
        close(pipes[1]);
    } else { //parent
        if (fork() == 0) { //child
            close(pipes[1]);
            while (1) {
                char buf[50];
                int ret = read(pipes[0], buf, sizeof(buf));
                if (ret <= 0) break;
                sscanf(buf, "%d%c%d", &operandA, &_operator, &operandB);
                printf("%d%c%d=%d\n", operandA, _operator, operandB, eval(operandA, _operator, operandB));
            }
            close(pipes[0]);
        } else {
            close(pipes[0]);
            close(pipes[1]);
            wait(NULL);
        }

    }

    return 0;
}
