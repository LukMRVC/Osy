//***************************************************************************
//
// Program example for labs in subject Operating Systems
//
// Petr Olivka, Dept. of Computer Science, petr.olivka@vsb.cz, 2017
//
// Posix message queue example.
//
// The first process creates message queue and it start to work as producer.
// All others processes will connect to message queue and they will consume
// data from queue.
//
//***************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// global queue ID
int glb_msg_id = -1;

// first process
int glb_first = 0;

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

void clean_queue()
{
    log_msg( LOG_INFO, "Deleting of message queue..." );
    int ret = msgctl( glb_msg_id, IPC_RMID, NULL );
    if ( ret < 0 )
        log_msg( LOG_ERROR, "Unable to delete message queue!" );
    else
        log_msg( LOG_INFO, "Message queue deleted." );
    glb_msg_id = -1;
    return;
}

void clean( void )
{
    log_msg( LOG_INFO, "Final cleaning ..." );

    if ( glb_msg_id < 0 ) return;

    if ( glb_first )
    {
        log_msg( LOG_INFO, "This process was first and now it will try remove queue ..." );
        clean_queue();
   }
}

// catch signals
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
            "  Message queue example.\n"
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
        log_msg( LOG_INFO, "Clean message queue." );
        clean_queue();
        exit( 0 );
    }
}

//***************************************************************************

// my message ID
#define MYMSGID 4

void producer()
{
    static int count = 0;

    printf( "Produced %d\r", ++count );
    fflush( stdout );

    // send message - MYID + message number
    // Message length is without type of message !
    long snd[ 2 ] = { MYMSGID, count };

    log_msg( LOG_DEBUG, "Sending message in non-blocking mode ..." );
    int ret = msgsnd( glb_msg_id, &snd, sizeof( int ), IPC_NOWAIT );

    if ( ret < 0 )
    {
        if ( errno == EAGAIN )
        {
            printf( "\n" );
            usleep( 500000 );
            log_msg( LOG_INFO, "Queue is full. Sending in blocking mode ..." );
            ret = msgsnd( glb_msg_id, &snd, sizeof( int ), 0 );
        }
        else
        {
          log_msg( LOG_ERROR, "Unable to send message!" );
          exit( 1 );
        }
    }
}


void consumer()
{
    static int count = 0;

    long rcv[ 2 ];
    log_msg( LOG_DEBUG, "Receiving message in non blocking mode..." );
    int ret = msgrcv( glb_msg_id, &rcv, sizeof( int ), 0, IPC_NOWAIT );

    if ( ret < 0 )
    {
        if ( errno == ENOMSG )
        {
            printf( "\n" );
            log_msg( LOG_INFO, "Queue is empty. Waiting for message in blocking mode ..." );
            usleep( 500000 );
            ret = msgrcv( glb_msg_id, &rcv, sizeof( int ), 0, 0 );
        }
        else
        {
            log_msg( LOG_ERROR, "Unable to receive message!" );
            exit( 1 );
        }
    }

    printf( "Consumed %d - received product %ld\r", ++count, rcv[ 1 ] );
    fflush( stdout );
}

int main( int num, char **arg )
{
    help( num, arg );

    // For the message ID will be used user ID.
    int msg_key = getuid();

    if ( msg_key == 0 )
    // root must define random ID
        msg_key = rand() % 1000 + 1;

    // get message queue
    glb_msg_id = msgget( msg_key, 0666 );

    if ( glb_msg_id < 0 )
    {
        // message queue probably not created yet
        log_msg( LOG_ERROR, "Unable to open message queue. Create new one..." );

        // message queue creation
        glb_msg_id = msgget( msg_key, 0666 | IPC_CREAT );
        if ( glb_msg_id < 0 )
        {
            log_msg( LOG_ERROR, "Unable to create message queue!" );
            return 1;
        }

        // this process created msg queue and it will delete it at exit
        glb_first = 1;
        log_msg( LOG_INFO, "This process created message queue and it will work as 'producer'." );
        log_msg( LOG_INFO, "The message queue will be deleted at exit." );
    }

    log_msg( LOG_DEBUG, "Message queue ID is = %d", glb_msg_id );

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
        if ( glb_first )
            producer();
        else
            consumer();
    }

    return 0;
}
