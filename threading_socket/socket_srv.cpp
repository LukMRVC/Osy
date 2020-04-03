//***************************************************************************
//
// Program example for labs in subject Operating Systems
//
// Petr Olivka, Dept. of Computer Science, petr.olivka@vsb.cz, 2017
//
// Example of socket server.
//
// This program is example of socket server and it allows to connect and serve
// the only one client.
// The mandatory argument of program is port number for listening.
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

#define STR_CLOSE   "close"
#define STR_QUIT    "quit"

//***************************************************************************
// log messages

#define LOG_ERROR               0       // errors
#define LOG_INFO                1       // information and notifications
#define LOG_DEBUG               2       // debug messages

// debug flag
int debug = LOG_INFO;

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

void help( int num, char **arg )
{
    if ( num <= 1 ) return;

    if ( !strcmp( arg[ 1 ], "-h" ) )
    {
        printf(
            "\n"
            "  Socket server example.\n"
            "\n"
            "  Use: %s [-h -d] port_number\n"
            "\n"
            "    -d  debug mode \n"
            "    -h  this help\n"
            "\n", arg[ 0 ] );

        exit( 0 );
    }

    if ( !strcmp( arg[ 1 ], "-d" ) )
        debug = LOG_DEBUG;
}

int eval(int a, char op, int b) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
        case '%': return a % b;
    }
}

//***************************************************************************

int main( int argn, char **arg )
{
    if ( argn <= 1 ) help( argn, arg );

    int port = 0;

    // parsing arguments
    for ( int i = 1; i < argn; i++ )
    {
        if ( !strcmp( arg[ i ], "-d" ) )
            debug = LOG_DEBUG;

        if ( !strcmp( arg[ i ], "-h" ) )
            help( argn, arg );

        if ( *arg[ i ] != '-' && !port )
        {
            port = atoi( arg[ i ] );
            break;
        }
    }

    if ( port <= 0 )
    {
        log_msg( LOG_INFO, "Bad or missing port number %d!", port );
        help( argn, arg );
    }

    log_msg( LOG_INFO, "Server will listen on port: %d.", port );

    // socket creation
    int sock_listen = socket( AF_INET, SOCK_STREAM, 0 );
    if ( sock_listen == -1 )
    {
        log_msg( LOG_ERROR, "Unable to create socket.");
        exit( 1 );
    }

    in_addr addr_any = { INADDR_ANY };
    sockaddr_in srv_addr;
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons( port );
    srv_addr.sin_addr = addr_any;

    // Enable the port number reusing
    int opt = 1;
    if ( setsockopt( sock_listen, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof( opt ) ) < 0 )
      log_msg( LOG_ERROR, "Unable to set socket option!" );

    // assign port number to socket
    if ( bind( sock_listen, (const sockaddr * ) &srv_addr, sizeof( srv_addr ) ) < 0 )
    {
        log_msg( LOG_ERROR, "Bind failed!" );
        close( sock_listen );
        exit( 1 );
    }

    // listenig on set port
    if ( listen( sock_listen, 1 ) < 0 )
    {
        log_msg( LOG_ERROR, "Unable to listen on given port!" );
        close( sock_listen );
        exit( 1 );
    }

    log_msg( LOG_INFO, "Enter 'quit' to quit server." );

    // go!
    while ( 1 )
    {
        int sock_client = -1;

        while ( 1 ) // wait for new client
        {
            // set for handles
            fd_set read_wait_set;
            // empty set
            FD_ZERO( &read_wait_set );
            // add stdin
            FD_SET( STDIN_FILENO, &read_wait_set );
            // add listen socket
            FD_SET( sock_listen, &read_wait_set );

            int sel = select( sock_listen + 1, &read_wait_set, NULL, NULL, NULL );

            if ( sel < 0 )
            {
                log_msg( LOG_ERROR, "Select failed!" );
                exit( 1 );
            }

            if ( FD_ISSET( sock_listen, &read_wait_set ) )
            { // new client?
                sockaddr_in rsa;
                int rsa_size = sizeof( rsa );
                // new connection
                sock_client = accept( sock_listen, ( sockaddr * ) &rsa, ( socklen_t * ) &rsa_size );
                if (!fork()) {
                    if (sock_client == -1) {
                        log_msg(LOG_ERROR, "Unable to accept new client.");
                        close(sock_listen);
                        exit(1);
                    }
                    uint lsa = sizeof(srv_addr);
                    // my IP
                    getsockname(sock_client, (sockaddr *) &srv_addr, &lsa);
                    log_msg(LOG_INFO, "My IP: '%s'  port: %d",
                            inet_ntoa(srv_addr.sin_addr), ntohs(srv_addr.sin_port));
                    // client IP
                    getpeername(sock_client, (sockaddr *) &srv_addr, &lsa);
                    log_msg(LOG_INFO, "Client IP: '%s'  port: %d",
                            inet_ntoa(srv_addr.sin_addr), ntohs(srv_addr.sin_port));
//                    close(sock_listen);
                    break;
                }
                else
                    close(sock_client);
            }
            if ( FD_ISSET( STDIN_FILENO, &read_wait_set ) )
            { // data on stdin
                char buf[ 128 ];
                int len = read( STDIN_FILENO, buf, sizeof( buf) );
                if ( len < 0 )
                {
                    log_msg( LOG_DEBUG, "Unable to read from stdin!" );
                    exit( 1 );
                }

                log_msg( LOG_DEBUG, "Read %d bytes from stdin" );
                // request to quit?
                if ( !strncmp( buf, STR_QUIT, strlen( STR_QUIT ) ) )
                {
                    log_msg( LOG_INFO, "Request to 'quit' entered.");
                    close( sock_listen );
                    exit( 0 );
                }
            }
        } // while wait for client

        while ( 1  )
        { // communication
            char buf[ 256 ];
            int opA;
            int opB;
            char op;
            char resultStr[20];
            // set for handles
            fd_set read_wait_set;
            // empty set
            FD_ZERO( &read_wait_set );
            // add stdin
            FD_SET( STDIN_FILENO, &read_wait_set );
            // add client
            FD_SET( sock_client, &read_wait_set );
            timeval timeout;
            timeout.tv_sec = 2;
            timeout.tv_usec = 0;
            int sel = select( sock_client + 1, &read_wait_set, NULL, NULL, &timeout );

            if ( sel < 0 )
            {
                log_msg( LOG_ERROR, "Select failed!" );
                exit( 1 );
            }

            // data on stdin?
            if ( FD_ISSET( STDIN_FILENO, &read_wait_set ) )
            {
                // read data from stdin
                int l = read( STDIN_FILENO, buf, sizeof( buf ) );
                if ( l < 0 )
                        log_msg( LOG_ERROR, "Unable to read data from stdin." );
                else
                        log_msg( LOG_DEBUG, "Read %d bytes from stdin.", l );

                // send data to client
                l = write( sock_client, buf, l );
                if ( l < 0 )
                        log_msg( LOG_ERROR, "Unable to send data to client." );
                else
                        log_msg( LOG_DEBUG, "Sent %d bytes to client.", l );
            }
            // data from client?
            else if ( FD_ISSET( sock_client, &read_wait_set ) )
            {

                int l = readline( sock_client, buf, sizeof( buf ) );
                if ( !l )
                {
                        log_msg( LOG_DEBUG, "Client closed socket!" );
                        close( sock_client );
                        break;
                }
                else if ( l < 0 )
                        log_msg( LOG_DEBUG, "Unable to read data from client." );
                else
                        log_msg( LOG_DEBUG, "Read %d bytes from client.", l );

                // write data to client
//                l = write( STDOUT_FILENO, buf, l );
//                if ( l < 0 )
//                        log_msg( LOG_ERROR, "Unable to write data to stdout." );

                // close request?
                if ( !strncasecmp( buf, "close", strlen( STR_CLOSE ) ) )
                {
                    write(sock_client, buf, strlen(buf));
                    log_msg( LOG_INFO, "Client sent 'close' request to close connection." );
                    close( sock_client );
                    log_msg( LOG_INFO, "Connection closed. Waiting for new client." );
                    break;
                } else {
                    sscanf(buf, "%d%c%d", &opA, &op, &opB);
                    int result = eval(opA, op, opB);
                    sprintf(buf, "%d%c%d=%d\n", opA, op, opB, result);
                    write(sock_client, buf, strlen(buf));
                }

            }
            // request for quit
            if ( !strncasecmp( buf, "quit", strlen( STR_QUIT ) ) )
            {
                close( sock_listen );
                close( sock_client );
                log_msg( LOG_INFO, "Request to 'quit' entered" );
                exit( 0 );
            }
        } // while communication
    } // while ( 1 )

    return 0;
}
