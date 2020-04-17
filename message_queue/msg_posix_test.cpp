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
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <mqueue.h>

#define MQ_NAME                 "/mq_example"

// message queue fd
mqd_t glb_msg_fd = -1;

// first process?
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

void clean( void )
{
    log_msg( LOG_INFO, "Final cleaning ..." );

    if ( glb_msg_fd < 0 ) return;

    if ( glb_first )
    {
        log_msg( LOG_INFO, "This process was first and now it will try remove queue ..." );

        if ( mq_unlink( MQ_NAME ) < 0 )
            log_msg( LOG_ERROR, "Unable to delete message queue!" );
        else
            log_msg( LOG_INFO, "Message queue deleted." );
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
        mq_unlink( MQ_NAME );
        exit( 0 );
    }
}


//***************************************************************************

void producer()
{
    static int count = 0;

    printf( "Produced %d\r", ++count );
    fflush( stdout );

    int data = rand() % 1000;

    log_msg( LOG_DEBUG, "Producer will send message with timeout ..." );

    timeval curtime;
    gettimeofday( &curtime, NULL );
    timespec tout = { curtime.tv_sec + 1, curtime.tv_usec * 1000 };

    int ret = mq_timedsend( glb_msg_fd, ( const char * ) &data, sizeof( data ), 0, &tout );
    if ( ret < 0 )
    {
        if ( errno == ETIMEDOUT )
        {
            printf( "\n" );
            log_msg( LOG_INFO, "Message queue is full. Send without timeout ..." );
            ret = mq_send( glb_msg_fd, ( const char * ) &data, sizeof( data ), 0 );
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

    int data;

    log_msg( LOG_DEBUG, "Receive message with timeout ..." );

    timeval curtime;
    gettimeofday( &curtime, NULL );
    timespec tout = { curtime.tv_sec + 1, curtime.tv_usec * 1000 };

    int ret = mq_timedreceive( glb_msg_fd, ( char * ) &data, sizeof( data ), NULL, &tout );

    if ( ret < 0 )
    {
        if ( errno == ETIMEDOUT )
        {
            printf( "\n" );
            log_msg( LOG_INFO, "No message. Wait for message without timeout ..." );
            usleep( 500000 );
            ret = mq_receive( glb_msg_fd, ( char * ) &data, sizeof( data ), NULL );
        }
        else
        {
            log_msg( LOG_ERROR, "Unable to receive message!" );
            exit( 1 );
        }
    }

    printf( "Consumed %d messages\r", ++count );
    fflush( stdout );
}

int main( int num, char **arg )
{
    // open message queue
    glb_msg_fd = mq_open( MQ_NAME, O_RDWR );

    if ( glb_msg_fd < 0 )
    {
        // message queue probably not created yet
        log_msg( LOG_ERROR, "Unable to open message queue. Create new one...." );

        mq_attr mqa;
        bzero( &mqa, sizeof( mqa ) );
//        mqa.mq_flags = ;
        mqa.mq_maxmsg = 8;
        mqa.mq_msgsize = sizeof( int );qm

        // message queue creation
        glb_msg_fd = mq_open( MQ_NAME, O_RDWR | O_CREAT, 0660, &mqa );
        if ( glb_msg_fd < 0 )
        {
            log_msg( LOG_ERROR, "Unable to create message queue!" );
            return 1;
        }

        // this process created mq and it will delete it at exit.
        glb_first = 1;
        log_msg( LOG_INFO, "This process created message queue and it will work as 'producer'." );
        log_msg( LOG_INFO, "The message queue will be deleted at exit." );
    }

    log_msg( LOG_DEBUG, "FD of message queue is %d", glb_msg_fd );

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
