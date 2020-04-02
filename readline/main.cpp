#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <string.h>
#include "BufferedFileDescriptorReader.h"

#define TIMEOUT 500000


int main() {

    int roura[2];
    pipe(roura);

    BufferedFileDescriptorReader reader(roura[0]);
    const int bufSize = 50;
    char buf[bufSize];

    if (!fork()) {
        close(roura[0]);
        while (1) {
            int sum = getpid();
            dprintf(roura[1], "%d ", sum);
            int max = rand() % 5 + 1;
            for (int i = 0; i < max; ++i) {
                int num = rand() % 100;
                dprintf(roura[1], "%d ", num);
                sum += num;
            }
            dprintf(roura[1], "%d\n", sum);
            usleep(TIMEOUT - 200000);
        }
        close(roura[1]);
        exit(0);
    }
    int read_bytes;
    close(roura[1]);
    while (1) {
        read_bytes = reader.readline(buf, bufSize);
        if (read_bytes <= 0)
            break;
        printf("%s", buf);
        fflush(stdout);
        usleep(TIMEOUT);
    }

    close(roura[0]);

    return 0;
}


