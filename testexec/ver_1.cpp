/*
char * const arg[] = {"ls", "-la", "/tmp", NULL};
//nulty arg musi byt nazev toho programu, takze to je redundantni
//    execlp("ls", "ls", "-la", "/tmp", NULL);
execvp(arg[0] , arg);
printf("Pokud jsem zde, neco je spatne \n");
exit(0);*/

int pipes1[2];
pipe(pipes1);

if ( !fork() ) { //potomek
    close(pipes1[0]);
    dup2(pipes1[ 1 ], 1); //roura out na stdout

    close(pipes1[1]);
    execlp("ls", "ls", NULL);

    printf("Tady neco nehraje\n");

    exit(0);
}

if ( !fork() ) {
close(pipes1[1]);

dup2(pipes1[0], 0); //roura na stdin

close(pipes1[0]);
close(1);
int fd = open("soubor", O_RDWR | O_CREAT | O_TRUNC, 0644);
dup2(fd, 1); //soubor na stdout
close(fd);

execlp("tr", "tr", "a-z", "A-Z", NULL);

printf("Neco je tady spatne\n");

exit(0);
}


close(pipes1[ 1 ]);
//    while ( 1 ) {
//        char buf[50];
//        int l = read(pipes1[0], buf, sizeof(buf));
//        if (l <= 0) break;
//        for (int i = 0; i < l; ++i) {
//            buf[i] = toupper( buf[i] );
//        }
//        write(1, buf, l);
//    }
close(pipes1[ 0 ]);

wait(NULL );//
// Created by lukas on 06.03.20.
//

