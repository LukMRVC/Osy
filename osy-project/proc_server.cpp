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
#include <time.h>
#include "lib/Utils.h"
#include "lib/BufferedFileDescriptorReader.h"

int main(int argc, char ** argv) {
    ProgramArgs args = ProgramArgs::parse(argc, argv, ProgramArgs::server);
    Logger logger(STDOUT_FILENO, args.log_level);
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    const int freeSpace = 10;
    int taken = 0;

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

    BufferedFileDescriptorReader stdinReader(STDIN_FILENO, 512);


    int client_fd;
    //begin work  => connection section
    while ( 1 ) {
        fd_set read_set;
        FD_ZERO(&read_set);
        FD_SET( STDIN_FILENO, &read_set);
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
            if (client_fd == -1) {
                logger.log(log_error, "Unable to accept new client");
                close(server_socket);
                exit(1);
            }

            if ( !fork() ) { //child process code
                uint lsa = sizeof(sock_address);
                getsockname(client_fd, (sockaddr *) &sock_address, &lsa);
                logger.log(log_info, "Server IP: '%s' port: %d", inet_ntoa(sock_address.sin_addr), ntohs(sock_address.sin_port));
                getpeername(client_fd, (sockaddr *) &sock_address, &lsa);
                logger.log(log_info, "Client IP: '%s' port: %d", inet_ntoa(sock_address.sin_addr), ntohs(sock_address.sin_port));
                close(server_socket);
                break; //break the loop and go to communication section
            } else {
                logger.log(log_info, "Parent PID: %d", getpid());
                close(client_fd);
            }
        }
        //Data on stdin?
        if (FD_ISSET(STDIN_FILENO, &read_set)) {
            char line[255];
            int read_bytes = stdinReader.readline(line, 254);
            if (read_bytes < 0) {
                logger.log(log_debug, "Unable to read data from stdin");
            }
            logger.log(log_debug, "Read %d bytes from stdin", read_bytes);
            logger.log(log_debug, "STDIN: %s", line);
            if ( !strcasecmp(line, "quit")) {
                logger.log(log_info, "Quit request entered, quitting...");
                close(server_socket);
                exit(0);
            }
        }
    }

    //Communication section
    if (client_fd != -1) {
        logger.log(log_info, "Child PID: %d", getpid());
        srand(time(nullptr));
        BufferedFileDescriptorReader reader(client_fd, 1024);
        logger.log(log_info, "Client connected on FD %d\n", client_fd);
        fd_set read_set;
        while ( 1 ) {
            FD_ZERO(&read_set);
            FD_SET( STDIN_FILENO, &read_set);
            FD_SET( client_fd, &read_set );
            logger.log(log_info, "Waiting for client!\n");
            int sel = select(client_fd + 1, &read_set, NULL, NULL, NULL);
            if (sel < 0) {
                logger.log(log_error, "Select failed!");
                exit(1);
            }
            //Data on stdin?
            if (FD_ISSET(STDIN_FILENO, &read_set)) {
                char line[255];
                logger.log(log_debug, "Data on STDIN");
                int read_bytes = stdinReader.readline(line, 254);
                if (read_bytes < 0) {
                    logger.log(log_debug, "Unable to read data from stdin");
                }
                logger.log(log_debug, "Read %d bytes from stdin", read_bytes);
                logger.log(log_debug, "STDIN: %s", line);
                if ( !strcasecmp(line, "quit")) {
                    logger.log(log_info, "Quit request entered, quitting...");
                    close(server_socket);
                    exit(0);
                }
            }

            //Data from client
            if (FD_ISSET(client_fd, &read_set)) {
                logger.log(log_debug, "Data from client");
                char line[256];
                char messageStr[256];
                int read_bytes = reader.readline(line, 255);
                if (read_bytes < 0) {
                    logger.log(log_debug, "Unable to read data from client");
                }
                logger.log(log_info, "Server received: %s", line);
                Message payload;
                if (Message::parse_message(line, payload)) {
                    switch (payload.code) {
                        case Command::INCOMING:
                            if (taken >= freeSpace) {
                                Message bye(Answer::TYPE, Answer::NO_SPACE, Answer::S_NO_SPACE);
                                bye.send(client_fd);
                            } else {
                                logger.log(log_debug, "Welcoming client");
                                Message welcome(Answer::TYPE, Answer::SIT, Answer::S_SIT);
                                testvalue(welcome.send(client_fd, taken++), -1, logger, "Unable to write");
                            }
                            break;
                        case Command::HUNGRY: {
                            usleep(rand() % 100000 + 5000);
                            Message eat(Answer::TYPE, Answer::EAT, Answer::S_EAT);
                            eat.send(client_fd);
                        }
                            break;

                        case Command::LEAVING: {
                            Message bye(Answer::TYPE, Answer::BYE, Answer::S_BYE);
                            bye.send(client_fd);
                            --taken;
                            close(client_fd);
                            exit(0);
                        }
                            break;

                        case Command::FULL:
                            Message fork(Answer::TYPE, Answer::FORK, Answer::S_FORK);
                            fork.send(client_fd);
                            break;
                    }
                } else {
                    payload.type = Error::TYPE;
                    payload.code = Error::INVALID_MESSAGE;
                    strcpy(payload.text, Error::S_INVALID_MESSAGE);
                    payload.to_string(messageStr);
                    write(client_fd, messageStr, strlen(messageStr));
                }
            }
        }


    }


    return 0;
}