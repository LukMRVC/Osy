#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <cctype>
#include <sys/wait.h>
#include <fcntl.h>
#include <string>
#include <cstring>

int main(int argc, char** argv) {
    int roura[2];
    pipe(roura);

    if (argc == 1)
        exit(-1);

    if ( ! fork() ) { //potomek
        close(roura[0]);

        dup2(roura[1], 1);
        close(roura[1]);

        execl("../gen", "gen", argv[1], NULL);
        printf("Neco je spatne kamo\n");
        exit(0);
    }
    int roura2[2];
    pipe(roura2);

    if ( !fork() ) {
        close(roura[1]);
        dup2(roura[0], 0);
        close(roura[0]);
        if (argc >= 3) {
            close(roura2[0]);
            dup2(roura2[1], 2);
            close(roura[1]);
            execl("../scan", "scan", argv[2], NULL);
        }
        else {
            close(roura2[0]);
            close(roura2[1]);
            execl("../scan", "scan", NULL);
        }
        printf("Neco je tady spatne\n");
        exit(0);
    }

    close(roura2[1]);
    close(roura[1]);
    close(roura[0]);
    usleep(1000);
    if (argc >= 3) {
        if (strcmp(argv[2], "-e") == 0) {
            int num = 0;
            int sum = 0;
            int numCount = 0;
            while (1) {
                char buf[50];
                int l = read(roura2[0], buf, sizeof(buf));
                if (l <= 0) break;
                buf[l] = '\0';
                sscanf(buf, "%d", &num);
//                printf("%d\n", num);
                sum += num;
                numCount++;
            }
            printf("\nPrumerny vysledku je: %.2f\n", (float)sum / numCount);
        }
    }
    close(roura2[0]);
    wait(NULL);

    exit(0);
}
