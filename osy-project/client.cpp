#include <stdio.h>
#include "lib/Message.h"
#include "lib/ProgramArgs.h"
#include "lib/Protocol.h"
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include "lib/BufferedFileDescriptorReader.h"
#include "lib/Utils.h"

int main(int argc, char ** argv) {
    ProgramArgs args = ProgramArgs::parse(argc, argv, ProgramArgs::client);
    printf("Hello, client_address world! Port: %d\n", args.port);
    Logger logger(STDOUT_FILENO, args.log_level);
    logger.log(log_debug, "Connecting to '%s':%d", args.host, args.port);

    addrinfo addrinfo_hint, *ai_res;
    bzero(&addrinfo_hint, sizeof(addrinfo_hint));
    addrinfo_hint.ai_family = AF_INET;
    addrinfo_hint.ai_socktype = SOCK_STREAM;

    int addr_in = getaddrinfo(args.host, NULL, &addrinfo_hint, &ai_res);
    if ( addr_in ) {
        logger.log(log_error, "Unknown host name");
        exit( 1 );
    }
    sockaddr_in client_address = *( sockaddr_in * ) ai_res->ai_addr;
    client_address.sin_port = htons(args.port);
    freeaddrinfo(ai_res);
    int socket_client = socket(AF_INET, SOCK_STREAM, 0);
    testvalue(socket_client, -1, logger, "Unable to create client_address socket");

    //actual socket connection
    if ( connect(socket_client, (sockaddr *) &client_address, sizeof(client_address) ) < 0 ) {
        logger.log(log_error, "Unable to connection client socket to server");
        exit(1);
    }

    uint addresslen = sizeof(client_address);

    int get = getsockname(socket_client, (sockaddr *) &client_address, &addresslen );
    testvalue(get, -1, logger, "Unable to get socket name!");

    logger.log(log_debug, "My IP: '%s' port: %d",
            inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
    get = getpeername(socket_client,  (sockaddr *) &client_address, &addresslen);
    testvalue(get, -1, logger, "Unable to get peer name!");

    logger.log(log_debug, "Server IP: '%s' port: %d",
            inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

    BufferedFileDescriptorReader reader(socket_client);
    BufferedFileDescriptorReader stdinReader(STDIN_FILENO);

    while ( 1 ) {
        fd_set descriptor_set;
        FD_ZERO(&descriptor_set);
        FD_SET(STDIN_FILENO, &descriptor_set);
        FD_SET(socket_client, &descriptor_set);

        int sel = select(socket_client + 1, &descriptor_set, NULL, NULL, NULL);
        if (sel < 0) {
            logger.log(log_error, "Unable to select from descriptor set");
            exit(1);
        }

        if (FD_ISSET(STDIN_FILENO, &descriptor_set)) {
            char line[256];
            int read = stdinReader.readline(line, 255);
            write(socket_client, line, read);
        }

        if (FD_ISSET(socket_client, &descriptor_set)) {
            char line[256];
            int read = reader.readline(line, 256);
            if (read < 0) {
                logger.log(log_error, "Unable to read data from server");
                exit(1);
            }
            logger.log(log_debug, "Received: %s", line);
            Message received;
            if (Message::parse_message(line, received)) {
                switch (received.code) {
                    case Answer::BYE:
                        close(socket_client);
                        exit(0);
                        break;

                }
            }

        }

    }



    return 0;
}
