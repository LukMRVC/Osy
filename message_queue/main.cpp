#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <mqueue.h>

#define MQ_NAME                 "/mq_example"

// message queue fd
mqd_t glb_msg_fd = -1;

// first process?
int glb_first = 0;
#define SHM_NAME        "/shm_example"

// data structure for shared memory
struct shm_data
{
    int num_of_process;
    int counter;
};

// pointer to shared memory
shm_data *glb_data = NULL;

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

    if ( !glb_data ) return;

    int num_proc = -1;

    if ( glb_data != NULL )
    {
        glb_data->num_of_process--;
        num_proc = glb_data->num_of_process;
    }

    log_msg( LOG_INFO, "Shared memory releasing..." );
    int ret = munmap( glb_data, sizeof( *glb_data ) );
    if ( ret )
        log_msg( LOG_ERROR, "Unable to release shared memory!" );
    else
        log_msg( LOG_INFO, "Share memory released." );

    if ( num_proc == 0  )
    {
        log_msg( LOG_INFO, "This process is last (pid %d).", getpid() );
        shm_unlink( SHM_NAME );
    }

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
                "  Share memory use example.\n"
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
        shm_unlink( SHM_NAME );
        log_msg( LOG_INFO, "Shared memory cleaned." );
    }
}

//*****
int main(int num, char **arg) {
    help( num, arg );
    int first = 0;
    int fd = shm_open( SHM_NAME, O_RDWR, 0660 );

    if ( fd < 0 )
    {
        log_msg( LOG_ERROR, "Unable to open file for shared memory." );
        fd = shm_open( SHM_NAME, O_RDWR | O_CREAT, 0660 );
        if ( fd < 0 )
        {
            log_msg( LOG_ERROR, "Unable to create file for shared memory." );
            exit( 1 );
        }
        ftruncate( fd, sizeof( shm_data ) );
        log_msg( LOG_INFO, "File created, this process is first" );
        first = 1;
    }

    // share memory allocation
    glb_data = ( shm_data * ) mmap( NULL, sizeof( shm_data ), PROT_READ | PROT_WRITE,
                                    MAP_SHARED, fd, 0 );
    if ( !glb_data )
    {
        log_msg( LOG_ERROR, "Unable to attach shared memory!" );
        exit( 1 );
    }
    else
        log_msg( LOG_INFO, "Shared memory attached.");

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
    if ( first )
    {
        glb_data->counter = 0;
        glb_data->num_of_process = 1;
    }
    int old = glb_data->counter;
    log_msg( LOG_DEBUG, "Current global counter is %d.", glb_data->counter );

    while ( 1 )
    {
        if ( old != glb_data->counter )
            log_msg( LOG_INFO, "Another process changed global counter. Difference=%d", glb_data->counter - old );

        old = ++glb_data->counter;

        printf( "New value of global counter %d\r", glb_data->counter );
        fflush( stdout );

        if ( !( glb_data->counter % 100 ) ) usleep( 250000 );
    }

    return 0;
}
