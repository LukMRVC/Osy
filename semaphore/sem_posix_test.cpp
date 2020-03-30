//***************************************************************************
//
// Program example for labs in subject Operating Systems
//
// Petr Olivka, Dept. of Computer Science, petr.olivka@vsb.cz, 2017
//
// Example of posix semaphores.
// The first process will creates two semaphores.
// One semaphore will protect artificial critical section.
// The second semaphore is used as process number counter.
// The process which exits last will clean semaphores.
//
//***************************************************************************

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

#define SEM_MUTEX_NAME      "/sem_mutex"
#define SEM_COUNTER_NAME        "/sem_counter"

sem_t *sem_mutex = NULL;
sem_t *sem_counter = NULL;

//***************************************************************************
// log messages

#define LOG_ERROR               0       // errors
#define LOG_INFO                1       // information and notifications
#define LOG_DEBUG               2       // debug messages

// debug flag
int debug = LOG_INFO;

void log_msg( int log_level, const char *form, ... )
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

//***************************************************************************
// help

void help( int num, char **arg )
{
    if ( num <= 1 ) return;

    if ( !strcmp( arg[ 1 ], "-h" ) )
    {
        printf(
            "\n"
            "  Semaphore example.\n"
            "\n"
            "  Use: %s [-d -h -r]\n"
            "\n"
            "    -h  this help\n"
            "    -d  debug mode \n"
            "    -r  clean shared memory \n"
            "\n", arg[ 0 ] );

        exit( 0 );
    }

    if ( !strcmp( arg[ 1 ], "-d" ) )
        debug = LOG_DEBUG;

    if ( !strcmp( arg[ 1 ], "-r" ) )
    {
        log_msg( LOG_INFO, "Clean semaphores." );
        sem_unlink( SEM_COUNTER_NAME );
        sem_unlink( SEM_MUTEX_NAME );
        exit( 0 );
    }
}

//***************************************************************************

int main( int num, char **arg )
{
    help( num, arg );

    sem_counter = sem_open( SEM_COUNTER_NAME, O_RDWR );
    if ( !sem_counter )
    {
        // semaphore probably not created yet
        log_msg( LOG_ERROR, "Unable to open semaphore. Create new one." );

        // semaphores creation
        sem_mutex = sem_open( SEM_MUTEX_NAME, O_RDWR | O_CREAT, 0660, 1 );
        sem_counter = sem_open( SEM_COUNTER_NAME, O_RDWR | O_CREAT, 0660, 0 );
        if ( !sem_counter || !sem_mutex )
        {
            log_msg( LOG_ERROR, "Unable to create two semaphores!" );
            return 1;
        }
        log_msg( LOG_INFO, "Semaphores created." );
    }
    else
        sem_mutex = sem_open( SEM_MUTEX_NAME, O_RDWR );

    if ( !sem_mutex || !sem_counter )
    {
        log_msg( LOG_INFO, "Semaphores not available!" );
        return 1;
    }


    log_msg( LOG_DEBUG, "Increase number of processes." );
    if ( sem_post( sem_counter ) < 0 )
        log_msg( LOG_ERROR, "Unable to increase number of semaphores!" );

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

    while ( 1 )
    {
        // try to enter into critical section
        log_msg( LOG_DEBUG, "Try to enter into critical section ..." );
        if ( sem_trywait( sem_mutex ) < 0 )
        {
            log_msg( LOG_DEBUG,  "Critical section is occupied now. Wait for it ..." );

            if ( sem_wait( sem_mutex ) < 0 )
            {
                log_msg( LOG_ERROR, "Unable to enter into critical section!" );
                return 1;
            }
        }

        log_msg( LOG_DEBUG, "... process is now in critical section..." );

        // work in critical section
        printf( "CRITICAL SECTION:  " );
        for ( int cnt = 0; cnt < 50; cnt++ )
        {
            printf( "-" );
            fflush( stdout );
            usleep( 20000 );
        }
        printf( "\n" );

        log_msg( LOG_DEBUG, "... leaving critical section ..." );

        // unlock critical section
        if ( sem_post( sem_mutex ) < 0 )
        {
            log_msg( LOG_ERROR, "Unable to unlock critical section!" );
            return 1;
        }

        log_msg( LOG_DEBUG, "Critical section leaved." );

        printf( "Counting      : " );
        for ( int cnt = 0; cnt < 50; cnt++ )
        {
            printf( "+" );
            fflush( stdout );
            usleep( 20000 );
        }
        printf( "\n" );
    }

    return 0;
}
