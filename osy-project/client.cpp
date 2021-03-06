#include <stdio.h>
#include "lib/Message.h"
#include "lib/ProgramArgs.h"
#include "lib/Protocol.h"
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "lib/BufferedFileDescriptorReader.h"
#include "lib/Utils.h"
#include <time.h>
#include "lib/Philosopher.h"

int await_message(int code, char * line, BufferedFileDescriptorReader & reader, Message & mes, Logger & logger,
        const char * err_msg = "" );

int main(int argc, char ** argv) {
    ProgramArgs args = ProgramArgs::parse(argc, argv, ProgramArgs::client);
//    printf("Hello, client_address world! Port: %d\n", args.port);
    Logger logger(STDOUT_FILENO, args.log_level);
    logger.log(log_debug, "Connecting to '%s':%d", args.host, args.port);
    srand(time(nullptr));
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
        logger.log(log_error, "Unable to make connection between client socket to server");
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

    Philosopher philosopher;
    //Philosopher
    char line[256];
    Message received;
    //Philosopher tries to sit
    Message coming(Command::TYPE, Command::INCOMING, Command::S_INCOMING);
    coming.send(socket_client);
    if (reader.readline(line, 255)  < 0) {
        logger.log(log_error, "Unable to read data from socket");
        exit(1);
    }
    int rand_time;
    int rand_rounds = rand() % 6 + 1;


    if (Message::parse_message(line, received) && received.code == Answer::SIT) {
        //sitting
        for (int i = 0; i < rand_rounds; ++i ) {
            rand_time = rand() % 7 + 1;
            logger.log(log_plain, "Philosopher will think for %d seconds", rand_time);
            philosopher.think(rand_time);
            logger.log(log_plain, "Philosopher is hungry");
            Message hungry(Command::TYPE, Command::HUNGRY, Command::S_HUNGRY);
            hungry.send(socket_client);
//            logger.log(log_debug, "Waiting for server answer");
            //await allow to eat
            await_message(Answer::EAT, line, reader, received, logger, "Not allowed to eat :(");
            //eat
            rand_time = rand() % 7 + 1;
            logger.log(log_plain, "Philosopher will eat for %d seconds", rand_time);
            philosopher.eat(rand_time);
            //philosopher is full
            Message full(Command::TYPE, Command::FULL, Command::S_FULL);
            full.send(socket_client);
            //await return of a fork
            await_message(Answer::FORK, line, reader, received, logger, "Forks had not been washed");
        }
        logger.log(log_plain, "Philosopher is LEAVING the table!\n\n");
        Message bye(Command::TYPE, Command::LEAVING, Command::S_LEAVING);
        bye.send(socket_client);
        //await for a good bye
        await_message(Answer::BYE, line, reader, received, logger, "Philosopher did not get good bye :(");
        close(socket_client);
    } else {
        logger.log(log_error, "Table has no space or received message is bad!");
        exit(1);
    }

    return 0;
}

int await_message(int code, char * line, BufferedFileDescriptorReader & reader, Message & mes, Logger & logger,
                  const char * err_msg ) {
    do {
        if (mes.type == Info::TYPE) {
            logger.log(log_plain, "Info message: %s", mes.text);
        }
        if (reader.readline(line, 255) <= 0) {
            logger.log(log_error, "Unable to read data from socket");
            exit(1);
        }
        if (!(Message::parse_message(line, mes) && (mes.code == code || mes.type == Info::TYPE))) {
            logger.log(log_error, err_msg);
            logger.log(log_error, "Received: %s", line);
            exit(1);
        }
    } while (mes.type == Info::TYPE); //if it info message, try again
    return 0;
}
