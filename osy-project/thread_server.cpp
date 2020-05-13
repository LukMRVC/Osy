//
// Created by Lukas on 11.05.2020.
//
#include <stdio.h>
#include "lib/Message.h"
#include "lib/ProgramArgs.h"
#include "lib/Protocol.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include "lib/Utils.h"

int main(int argc, char** argv) {
    ProgramArgs args = ProgramArgs::parse(argc, argv, ProgramArgs::server);
    Logger logger(STDOUT_FILENO);
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    testvalue(server_socket, -1, logger, "Unable to get server socket");

    sockaddr_in sock_address;
    sock_address.sin_family = AF_INET; //ipv4 family
    sock_address.sin_port = htons(args.port);
    sock_address.sin_addr.s_addr = INADDR_ANY; //listen for any address connection

    //bind socket to address
    int ret = bind(server_socket, (sockaddr * ) &sock_address, sizeof(sock_address));
    testvalue(ret, -1, logger, "Unable to bind socket");

    int option = 1;
    //enable port number reusing
    ret = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    testvalue(ret, -1, logger, "Unable to set reuseaddr option");

    ret = listen(server_socket, 1);
    testvalue(ret, -1, logger, "Unable to listen with server socket");

    int client_fd = -1;

    //begin work
    while ( 1 ) {
        fd_set read_set;
        FD_ZERO(&read_set);
        FD_SET ( STDIN_FILENO, &read_set);
        FD_SET( server_socket, &read_set );

        int sel = select(server_socket + 1, &read_set, NULL, NULL, NULL);
        if (sel < 0) {
            logger.log(log_error, "Select failed!");
            exit(1);
        }

        //data on server socket
        if (FD_ISSET(server_socket, &read_set)) {
            //accept incoming connections
            sockaddr_in incoming_sock;
            int incoming_sock_size = sizeof( incoming_sock );

            client_fd = accept(server_socket, (sockaddr *) &incoming_sock, (socklen_t *) &incoming_sock_size);




            testvalue(client_fd, -1, logger, "Unable to accept connection");
        }

    }



    return 0;
}

