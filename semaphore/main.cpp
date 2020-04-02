//
// Created by lukas on 27.03.20.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <semaphore.h>
#include <time.h>
#include <sys/wait.h>
#include "readline.cpp"

#define SEM_MUTEX_NAME      "/sem_mutex"
#define SEM_COUNTER_NAME        "/sem_counter"

sem_t *sem_mutex = NULL;
sem_t *sem_counter = NULL;

#define LOG_ERROR               0       // errors
#define LOG_INFO                1       // information and notifications
#define LOG_DEBUG               2       // debug messages
#define TIMEOUT                 100000
// debug flag
int debug = LOG_INFO;

void produce_lines(int roura);

void log_msg(int log_level, const char *form, ... )
{
    const char *out_fmt[] = {
            "ERR: (%d-%s) %s\n",
            "INF: %s\n",
            "DEB: %s\n" };

    if ( log_level && log_level > debug ) return;

    char buf[ 1024 ];
    va_list arg;
    va_start( arg, form );
    vsprintf( buf, form, arg );
    va_end( arg );

    switch ( log_level )
    {
        case LOG_INFO:
        case LOG_DEBUG:
            fprintf( stdout, out_fmt[ log_level ], buf );
            break;

        case LOG_ERROR:
            fprintf( stderr, out_fmt[ log_level ], errno, strerror( errno ), buf );
            break;
    }
}


//***************************************************************************
// cleaning function

void clean( void )
{
    log_msg( LOG_INFO, "Final cleaning ..." );

    // decrease number of processes
    if ( sem_trywait( sem_counter ) )
    {
        log_msg( LOG_ERROR, "Unable to decrease number of processes." );
        return;
    }

    int val;
    if ( sem_getvalue( sem_counter, &val ) < 0 )
    {
        log_msg( LOG_ERROR, "Unable to get number of processes." );
        return;
    }

    log_msg( LOG_INFO, "Number of remaining processes is %d", val );

    if ( val == 0 )
    {
        log_msg( LOG_INFO, "This process is last. Clean semaphores ..." );
        sem_unlink( SEM_COUNTER_NAME );
        sem_unlink( SEM_MUTEX_NAME );
    }
}

// catch signal
void catch_sig( int sig )
{
    exit( 1 );
}

void open_semaphores() {
    sem_counter = sem_open(SEM_COUNTER_NAME, O_RDWR);
    if ( !sem_counter ) {
        // semaphore probably not created yet
        log_msg( LOG_ERROR, "Unable to open semaphore. Create new one." );
        sem_mutex = sem_open(SEM_MUTEX_NAME, O_RDWR | O_CREAT, 0660, 1);
        sem_counter = sem_open( SEM_COUNTER_NAME, O_RDWR | O_CREAT, 0660, 0 );
        if (!sem_counter || !sem_mutex) {
            log_msg(LOG_ERROR, "Unable to create semaphores!");
            exit(0);
        }
        log_msg(LOG_INFO, "Semaphores created." );
    } else
        sem_mutex = sem_open(SEM_MUTEX_NAME, O_RDWR);

    if (!sem_counter || !sem_mutex) {
        log_msg(LOG_ERROR, "Semaphores not available!");
        exit(0);
    }
}

void produce_item(int & item) {
    printf("Production started!\n");
    int r = rand() % 1000;
    for ( int i = 0; i < 50; ++i) {
        printf("+");
        fflush(stdout);
        usleep(20000);
    }
    printf("\nDone producing!\n");
}

int consume_item(int & item) {
    printf("Consuming started!\n");
    for (int i = 0; i < 50; ++i) {
        printf("-");
        fflush(stdout);
        usleep(20000);
    }
    printf("\nDone consuming!\n");
}

void producer(int & item) {
    while ( true ) {
        if (sem_trywait( sem_mutex ) < 0) { //critical section occupied
            if (sem_wait( sem_mutex ) < 0) { //unable to enter critical section
                exit(0);
            }
        }
        produce_item(item);
        if (sem_post( sem_mutex ) < 0) { //unable to leave critical section
            log_msg(LOG_ERROR, "Unable to leave critical section...!!!");
            exit(0);
        }
        usleep(10);
    }
}

void consumer(int & item) {
    while (true) {
        if (sem_trywait( sem_mutex ) < 0) { //critical section occupied
            log_msg( LOG_INFO,  "Consumer Critical section is occupied now. Wait for it ..." );
            if (sem_wait(sem_mutex) < 0) { //unable to enter critical section
                exit(0);
            }
        }
        consume_item(item);
        if (sem_post(sem_mutex) < 0) {
            log_msg(LOG_ERROR, "Unable to leave critical section...!!!");
            exit(0);
        }
        usleep(10);
    }
}

int main(int argc, char ** argv) {
    int children = 2;

    if (argc >= 2)
        children = strtol(argv[1], NULL, 10);
    else {
        printf("Please specify the number of children as a integer program argument\n");
        exit(1);
    }

    srand(time(nullptr));

    open_semaphores();
    if ( sem_post( sem_counter ) < 0 )
        log_msg( LOG_ERROR, "Unable to increase number of semaphores!" );
    log_msg( LOG_DEBUG, "Increase number of processes." );


    struct sigaction sa;
    bzero( &sa, sizeof( sa ) );
    sa.sa_handler = catch_sig;
    sigemptyset( &sa.sa_mask );
    sa.sa_flags = 0;
    // catch sig <CTRL-C>
    sigaction( SIGINT, &sa, NULL );
    // catch SIG_PIPE
    sigaction( SIGPIPE, &sa, NULL );
    // clean at exit
    atexit( clean );
    int roura[2];
    pipe(roura);
    for (int i = 0; i < children; ++i) {
        if ( sem_post( sem_counter ) < 0 )
            log_msg( LOG_ERROR, "Unable to increase number of semaphores!" );
        if ( !fork() ) {
            srand(time(NULL) ^ getpid());
            close(roura[0]);
            produce_lines(roura[1]);
            close(roura[1]);
            exit(0);
        }
    }
    char line[512];
    close(roura[1]);
    int readBytes = 0;
    int number;
    int sum;
    int num_pos;
    int num_pos_sum;
    unsigned int readLines = 1;
    while ( 1 ) {
        if (readLines % 10 == 0) {
            if (sem_trywait(sem_mutex) < 0) {
                if (sem_wait(sem_mutex) < 0) {
                    log_msg(LOG_ERROR, "Unable to enter critical section");
                    exit(0);
                }
            }
            sleep(4);
            if (sem_post(sem_mutex) < 0) {
                log_msg(LOG_ERROR, "Unable to unlock critical section");
                exit(1);
            }
        }
        sum = 0;
        num_pos = 0;
        num_pos_sum = 0;
        readBytes = readline(roura[0], line, 512);
        readLines++;
        if (readBytes <= 0)
            break;
        while(sscanf((line + num_pos_sum), "%d%n", &number, &num_pos) == 1) {
            sum += number;
            num_pos_sum += num_pos;
        }
        if ((sum - number) != number) {
            printf("Radek %s neni spravne!!!\n", line);
        } else {
            printf("Radek %s je v poradku\n", line);
        }
        usleep(TIMEOUT);
    }
    close( roura[0] );
    wait(NULL);

    return 0;
}

void produce_lines(int roura) {
    while ( 1 ) {
        if (sem_trywait(sem_mutex) < 0) { //critical section occupied, wait
            if (sem_wait(sem_mutex) < 0) {
                log_msg(LOG_ERROR, "Unable to enter critical section");
                exit(0);
            }
        }
        int sum = getpid();
        dprintf(roura, "%d ", sum);
        int pocet = rand() % 9 + 1;
        for (int i = 0; i < pocet; ++i) {
            int num = rand() % 1000;
            sum += num;
            dprintf(roura, "%d ", num);
        }
        dprintf(roura, "%d\n", sum);
        if (sem_post(sem_mutex) < 0) {
            printf("Unable to unlock critical section");
            exit(0);
        }
        usleep(TIMEOUT);
    }
}
