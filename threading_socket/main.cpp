#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <pthread.h>
#include "BufferedFileDescriptorReader.h"

#define LOG_ERROR               0       // errors
#define LOG_INFO                1       // information and notifications
#define LOG_DEBUG               2       // debug messages

#define STR_CLOSE   "close"
#define STR_QUIT    "quit"

int debug = LOG_INFO;

void log_msg( int log_level, const char *form, ... );
void help( int num, char **arg );

void * client_communication(void * sockfd);


int main(int argn, char **arg) {

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

    if (port <= 0) {
        log_msg(LOG_INFO, "Bad or missing port number %d!", port);
        help (argn, arg);
    }

    log_msg( LOG_INFO, "Server will listen on port: %d.", port );

    int socket_server = socket( AF_INET, SOCK_STREAM, 0);

    if ( socket_server == -1 )
    {
        log_msg( LOG_ERROR, "Unable to create socket.");
        exit( 1 );
    }

    in_addr addr_any = { INADDR_ANY };
    sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons( port );
    server_addr.sin_addr = addr_any;

    int opt = 1;

    if (setsockopt(socket_server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof( opt )) < 0)
        log_msg( LOG_ERROR, "Unable to set socket option!" );

    if ( bind(socket_server, (const sockaddr * ) &server_addr, sizeof(server_addr )) < 0) {
        log_msg( LOG_ERROR, "Bind failed!" );
        close( socket_server );
        exit( 1 );
    }

    if (listen(socket_server, 5) < 0) {
        log_msg( LOG_ERROR, "Unable to listen on given port!" );
        close( socket_server );
        exit( 1 );
    }

    log_msg( LOG_INFO, "Enter 'quit' to quit server." );

    pthread_t client_thread;

    while ( 1 ) {

        int sock_client = -1;

        fd_set read_wait_set;
        FD_ZERO(&read_wait_set);
        FD_SET( STDIN_FILENO, &read_wait_set);
        FD_SET(socket_server, &read_wait_set);

        int selected = select(socket_server + 1, &read_wait_set, NULL, NULL, NULL);

        if ( selected < 0 )
        {
            log_msg( LOG_ERROR, "Select failed!" );
            exit( 1 );
        }

        if ( FD_ISSET( socket_server, &read_wait_set ) ) {
            //new client
            sockaddr_in rsa ;
            int rsa_size = sizeof(rsa);

            sock_client = accept( socket_server, (sockaddr *) &rsa, ( socklen_t * ) &rsa_size);
            if (sock_client == -1) {
                log_msg(LOG_ERROR, "Unable to accept new client.");
                close(socket_server);
                exit(1);
            }
            uint lsa = sizeof(server_addr);
            // my IP
            getsockname(sock_client, (sockaddr *) &server_addr, &lsa);
            log_msg(LOG_INFO, "My IP: '%s'  port: %d",
                    inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
            // client IP
            getpeername(sock_client, (sockaddr *) &server_addr, &lsa);
            log_msg(LOG_INFO, "Client IP: '%s'  port: %d",
                    inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
//                    close(sock_listen);
            pthread_create( &client_thread, NULL, client_communication, (void * ) (intptr_t) sock_client);
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
                close( socket_server );
                exit( 0 );
            }
        }
    }


    return 0;
}

void * client_communication(void * sockfd) {
    union Socket{
        void * sockf_d;
        int client;
    } socket;
    socket.sockf_d = sockfd;

    BufferedFileDescriptorReader reader(socket.client, 4096);

    while ( 1 ) {
        char buffer[255];
        // set for handles
        fd_set read_wait_set;
        // empty set
        FD_ZERO( &read_wait_set );
        // add stdin
//        FD_SET( STDIN_FILENO, &read_wait_set );
        // add client
        FD_SET( socket.client, &read_wait_set );
        int sel = select( socket.client + 1, &read_wait_set, NULL, NULL, NULL );
        if ( sel < 0 )
        {
            log_msg( LOG_ERROR, "Select failed!" );
            exit( 1 );
        }

        // data on stdin?
        if ( FD_ISSET( STDIN_FILENO, &read_wait_set ) ) {
            // read data from stdin
            int l = read( STDIN_FILENO, buffer, sizeof( buffer ) );
            if ( l < 0 )
                log_msg( LOG_ERROR, "Unable to read data from stdin." );
            else
                log_msg( LOG_DEBUG, "Read %d bytes from stdin.", l );

            // send data to client
            l = write( socket.client, buffer, l );
            if ( l < 0 )
                log_msg( LOG_ERROR, "Unable to send data to client." );

            // request for quit
            if ( !strncasecmp( buffer, "quit", strlen( STR_QUIT ) ) )
            {
                close( socket.client );
                log_msg( LOG_INFO, "Request to 'quit' entered" );
                exit( 0 );
            }



        } else if ( FD_ISSET( socket.client, &read_wait_set )  ) {
            int l = reader.readline(buffer, sizeof(buffer));
            if ( !l )
            {
                log_msg( LOG_DEBUG, "Client closed socket!" );
                close( socket.client );
                break;
            }
            else if ( l < 0 )
                log_msg( LOG_DEBUG, "Unable to read data from client." );

            if ( !strncasecmp( buffer, "close", strlen( STR_CLOSE ) ) ) {
                write(socket.client, buffer, strlen(buffer));
                log_msg(LOG_INFO, "Client sent 'close' request to close connection.");
                close(socket.client);
                log_msg(LOG_INFO, "Connection closed. Waiting for new client.");
                break;
            }
            int sum = 0;
            int number;
            int num_pos = 0;
            int num_pos_sum = 0;
            int line_number;
            printf("%s", buffer);
            sscanf(buffer, "%d%n", &line_number, &num_pos);
            num_pos_sum += num_pos;
            sum += line_number;
            while(sscanf((buffer + num_pos_sum), "%d%n", &number, &num_pos) == 1) {
                sum += number;
                num_pos_sum += num_pos;
            }
            if ((sum - number) != number) {
                dprintf(socket.client, "%d %d BAD\n", line_number, sum - number);
            } else {
                dprintf(socket.client, "%d %d OK\n", line_number, sum - number);
            }
        }
    }
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