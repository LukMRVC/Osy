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
#include "lib/Utils.cpp"

int main(int argc, char ** argv) {
    ProgramArgs args = ProgramArgs::parse(argc, argv, ProgramArgs::client);
    printf("Hello, client_address world! Port: %d\n", args.port);
    Logger logger(STDOUT_FILENO);
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



    return 0;
}
