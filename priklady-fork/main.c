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
    int pipes2[2];

    int operandA;
    int operandB;
    char _operator;

    char operands[] = { '+', '-', '*', '/', '%' };

    pipe(pipes);
    if (fork() == 0) { //child
        close(pipes[0]);
        for (int i = 0; i < 10; ++i) {
            char buf[40];
            sprintf(buf, "%d%c%d", rand() % 100, operands[rand() % 5], (rand() % 100) + 1);
            write(pipes[1], buf, strlen(buf));
            usleep(2000);
        }
        close(pipes[1]);
    } else { //parent
        pipe(pipes2);
        if (fork() == 0) { //child
            close(pipes2[0]);
            close(pipes[1]);

            while (1) {
                char buf[50];
                char resBuf[20];
                int ret = read(pipes[0], buf, sizeof(buf));
                if (ret <= 0) break;
                sscanf(buf, "%d%c%d", &operandA, &_operator, &operandB);
                int result = eval(operandA, _operator, operandB);
                sprintf(resBuf, "%d", result);
                write(pipes2[1], resBuf, strlen(resBuf));
                printf("%d%c%d=%d\n", operandA, _operator, operandB, result);
                usleep(1000);
            }
            close(pipes2[1]);
            close(pipes[0]);
        } else { //parent
            close(pipes[0]);
            close(pipes[1]);
            close(pipes2[1]);
            int sum = 0;
            int count = 1;
            int result;
            while (1) {
                char buf[20];
                int ret = read(pipes2[0], buf, sizeof(buf));
                if (ret <= 0) break;
                buf[ret] = '\0';
                sscanf(buf, "%d", &result);
                printf("Parent got result %d\n", result);
                sum += result;
                count++;
            }
            printf("Sum of results is: %d\n", sum);
            printf("Count of results is: %d\n", count);
            printf("Average of results is: %.2f\n", ( (float)sum / (float)count ));
            close(pipes2[0]);
        }

    }
    wait(NULL);
    return 0;
}
