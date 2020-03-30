#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

int main(void) {

    int pipes[2];
    pipe(pipes);
    int i = 0;
    srand(time(NULL));
    printf("Proccess PID=%d start\n", getpid());
    if (fork() == 0) { //child
        close(pipes[0]);
        for (; i < 1000; ++i) {
            char buf[100];
            sprintf(buf, "%d:%d\n", i, rand() % 1000);
            write(pipes[ 1 ], buf, strlen(buf));
            usleep(1000);
        }
        close(pipes[1]);
        printf("Child proccess PID=%d ends\n", getpid());
    } else {
        close(pipes[1]);
        while (1) {
            char buf[50];
            int ret = read(pipes[0], buf, sizeof(buf));
            if (ret <= 0) break;
            const char * str = "DJ Khaled, Another one\n";
            write(1, str, strlen(str));
            write(1, buf, ret);
        }
        close(pipes[ 0 ]);
        wait(NULL);
        printf("Main proccess PID=%d ends\n", getpid());
    }
    return 0;
}
