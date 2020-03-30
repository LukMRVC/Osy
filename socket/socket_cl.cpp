//***************************************************************************
//
// Program example for labs in subject Operating Systems
//
// Petr Olivka, Dept. of Computer Science, petr.olivka@vsb.cz, 2017
//
// Example of socket server.
//
// This program is example of socket client.
// The mandatory arguments of program is IP adress or name of server and
// a port number.
//
//***************************************************************************

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>

#define STR_CLOSE               "close"

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
// help

void help( int argn, char **arg )
{
    if ( argn <= 1 ) return;

    if ( !strcmp( arg[ 1 ], "-h" ) )
    {
        printf(
            "\n"
            "  Socket client example.\n"
            "\n"
            "  Use: %s [-h -d] ip_or_name port_number\n"
            "\n"
            "    -d  debug mode \n"
            "    -h  this help\n"
            "\n", arg[ 0 ] );

        exit( 0 );
    }

    if ( !strcmp( arg[ 1 ], "-d" ) )
        debug = LOG_DEBUG;
}

char* genNew() {
    char operands[5] = { '+', '-', '*', '/', '%' };
    char op = operands[rand() % 5];
    char *buf = (char*)malloc(sizeof(char) * 20);
    sprintf(buf, "%d%c%d\n", rand() % 10, op, (rand() % 10) + 1);
    return buf;
}

ssize_t readline(int fd, char* buf, int len) {
    ssize_t read_bytes = 0;
    while (true) {
        ssize_t ret = read(fd, (buf + read_bytes), 1);
        if (ret < 0)
            return -1;
        read_bytes++;
        if (ret == 0 || buf[read_bytes - 1] == '\n' || read_bytes == len)
            break;
    }
    return read_bytes;
}

//***************************************************************************

int main( int argn, char **arg )
{

    if ( argn <= 2 ) help( argn, arg );

    int port = 0;
    int total_sent = 0;
    int total_received = 0;
    char *host = NULL;

    // parsing arguments
    for ( int i = 1; i < argn; i++ )
    {
        if ( !strcmp( arg[ i ], "-d" ) )
            debug = LOG_DEBUG;

        if ( !strcmp( arg[ i ], "-h" ) )
            help( argn, arg );

        if ( *arg[ i ] != '-' )
        {
            if ( !host )
                host = arg[ i ];
            else if ( !port )
                port = atoi( arg[ i ] );
        }
    }

    if ( !host || !port )
    {
        log_msg( LOG_INFO, "Host or port is missing!" );
        help( argn, arg );
        exit( 1 );
    }

    log_msg( LOG_INFO, "Connection to '%s':%d.", host, port );

    addrinfo ai_req, *ai_ans;
    bzero( &ai_req, sizeof( ai_req ) );
    ai_req.ai_family = AF_INET;
    ai_req.ai_socktype = SOCK_STREAM;

    int get_ai = getaddrinfo( host, NULL, &ai_req, &ai_ans );
    if ( get_ai )
    {
      log_msg( LOG_ERROR, "Unknown host name!" );
      exit( 1 );
    }

    sockaddr_in cl_addr =  *( sockaddr_in * ) ai_ans->ai_addr;
    cl_addr.sin_port = htons( port );
    freeaddrinfo( ai_ans );

    // socket creation
    int sock_server = socket( AF_INET, SOCK_STREAM, 0 );
    if ( sock_server == -1 )
    {
        log_msg( LOG_ERROR, "Unable to create socket.");
        exit( 1 );
    }

    // connect to server
    if ( connect( sock_server, ( sockaddr * ) &cl_addr, sizeof( cl_addr ) ) < 0 )
    {
        log_msg( LOG_ERROR, "Unable to connect server." );
        exit( 1 );
    }

    uint lsa = sizeof( cl_addr );
    // my IP
    getsockname( sock_server, ( sockaddr * ) &cl_addr, &lsa );
    log_msg( LOG_INFO, "My IP: '%s'  port: %d",
             inet_ntoa( cl_addr.sin_addr ), ntohs( cl_addr.sin_port ) );
    // server IP
    getpeername( sock_server, ( sockaddr * ) &cl_addr, &lsa );
    log_msg( LOG_INFO, "Server IP: '%s'  port: %d",
             inet_ntoa( cl_addr.sin_addr ), ntohs( cl_addr.sin_port ) );

    log_msg( LOG_INFO, "Enter 'close' to close application." );

    // go!
    while ( 1 )
    {
        char buf[ 128 ];

        char* priklad;

        // set of handles
        fd_set read_wait_set;
        // clean set
        FD_ZERO( &read_wait_set );
        // add stdin
        FD_SET( STDIN_FILENO, &read_wait_set );
        // add socket
        FD_SET( sock_server, &read_wait_set );

        // select from handles
        if ( select( sock_server + 1, &read_wait_set, 0, 0, 0 ) < 0 ) break;



        // data on stdin?
        if ( FD_ISSET( STDIN_FILENO, &read_wait_set ) )
        {
            //  read from stdin
            int l = readline( STDIN_FILENO, buf, sizeof( buf ) );
            if ( l < 0 )
                log_msg( LOG_ERROR, "Unable to read from stdin." );
            else
                log_msg( LOG_DEBUG, "Read %d bytes from stdin.", l );

            // send data to server
            l = write( sock_server, buf, l );
            total_sent++;

            if ( l < 0 )
                log_msg( LOG_ERROR, "Unable to send data to server." );
            else
                log_msg( LOG_DEBUG, "Sent %d bytes to server.", l );
        }

        // data from server?
        if ( FD_ISSET( sock_server, &read_wait_set ) )
        {
            // read data from server
            int l = readline( sock_server, buf, sizeof( buf ) );
            total_received++;
            if ( !l )
            {
                log_msg( LOG_DEBUG, "Server closed socket." );
                break;
            }
            else if ( l < 0 )
                log_msg( LOG_DEBUG, "Unable to read data from server." );
            else
                log_msg( LOG_DEBUG, "Read %d bytes from server.", l );

            // display on stdout
            l = write( STDOUT_FILENO, buf, l );
            if ( l < 0 )
                log_msg( LOG_ERROR, "Unable to write to stdout." );

            // request to close?
            if ( !strncasecmp( buf, STR_CLOSE, strlen( STR_CLOSE ) ) )
            {
                log_msg( LOG_INFO, "Connection will be closed..." );
                break;
            }
        }
    }

    // close socket
    close( sock_server );
    printf("Totally send %d requests\n", total_sent);
    printf("Totally received %d responses\n", total_received);

    return 0;
  }
