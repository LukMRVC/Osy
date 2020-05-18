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
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>
#include <semaphore.h>

#define SHM_NAME "/shm_thread_chairs"

#define SEM_LOCK "/sem_thread_lock"
#define SEM_FORKS "/sem_thread_forks"

struct shm_chairs {
    int chair;
    int num_of_processes;
    int freeSpace;
};

struct thread_args {
    int sock;
    Logger *logger;
};

shm_chairs *shared_chairs = NULL;
void * client_communication (void * args);

sem_t * sem_forks = SEM_FAILED;
sem_t * sem_lock = SEM_FAILED;

void clean( void ) {
    Logger::print(log_debug, "Final cleaning of shared memory");

    if (!shared_chairs) return;

    int num_proc = -1;

    if (shared_chairs != NULL) {
        num_proc = --shared_chairs->num_of_processes;
    }
    Logger::print(log_debug, "Releasing shared memory");
    int ret = munmap(shared_chairs, sizeof(* shared_chairs));
    if (ret)
        Logger::print(log_error, "Unable to release shared memory!");
    else
        Logger::print(log_debug, "Shared memory released.");
    if (num_proc == 0) {
        Logger::print(log_info, "This process is last %d.", getpid());
        Logger::print(log_info, "Unbinding shared memory");
        shm_unlink(SHM_NAME);
    }

}

void catch_sig(int sig) {
    exit(1);
}

int main(int argc, char** argv) {
    ProgramArgs args = ProgramArgs::parse(argc, argv, ProgramArgs::server);
    Logger::s_log_level = args.log_level;
    Logger logger(STDOUT_FILENO, args.log_level);
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    constexpr int freeSpace = 2;
    int first = 0;

    sem_lock = sem_open(SEM_LOCK, O_RDWR | O_CREAT, 0600, 1);
    sem_forks = sem_open(SEM_FORKS, O_RDWR | O_CREAT, 0600, freeSpace + 1);
    if (sem_forks == SEM_FAILED || sem_lock == SEM_FAILED) {
        logger.log(log_error, "Unable to open semaphore");
        exit(1);
    }
    testvalue(sem_init(sem_forks, 1, freeSpace + 1), -1, logger, "Unable to initialize FORKS semaphore");
    testvalue(sem_init(sem_lock, 1, 1), -1, logger, "Unable to initialize LOCK semaphore");


    //Open shared memory
    int fd = shm_open(SHM_NAME, O_RDWR, 0660);
    if (fd < 0) {
        logger.log(log_error, "Unable to open shared memory");
        //Create shared memory
        fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0660);
        if (fd < 0) {
            logger.log(log_error, "Unable to create shared memory file");
            exit(1);
        }
        ftruncate(fd, sizeof(shared_chairs));
        logger.log(log_info, "Shared memory file created, this process if first");
        first = 1;
    }

    //Allocate shared memory
    shared_chairs = ( shm_chairs * ) mmap(NULL, sizeof(shm_chairs), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (!shared_chairs) {
        logger.log(log_error, "Unable to attach shared memory");
        exit(1);
    }
    else
        logger.log(log_info, "Shared memory attached.");

    if (first) {
        shared_chairs->chair = 0;
        shared_chairs->num_of_processes = 1;
        shared_chairs->freeSpace = freeSpace;
    }

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

    int client_fd;
    pthread_t client_thread;
    //begin work  => Connection section
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

            uint lsa = sizeof(sock_address);
//            shared_chairs->num_of_processes++;
            getsockname(client_fd, (sockaddr *) &sock_address, &lsa);
            logger.log(log_debug, "Server IP: '%s' port: %d", inet_ntoa(sock_address.sin_addr), ntohs(sock_address.sin_port));
            getpeername(client_fd, (sockaddr *) &sock_address, &lsa);
            logger.log(log_debug, "Client IP: '%s' port: %d", inet_ntoa(sock_address.sin_addr), ntohs(sock_address.sin_port));
            struct thread_args args = { client_fd, &logger };
            pthread_create( &client_thread, NULL, client_communication, (void *) &args);
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
            if ( !strcasecmp(line, "quit\n")) {
                logger.log(log_info, "Quit request entered, quitting...");
                close(server_socket);
                pthread_join(client_thread, NULL);
                exit(0);
            }
        }
    }

    return 0;
}

void * client_communication (void * args) {
    struct thread_args *arguments = (struct thread_args *)args;
    int socket = arguments->sock;
    Logger *logger = arguments->logger;
    srand(time(nullptr));
    BufferedFileDescriptorReader reader(socket, 1024);
    logger->log(log_debug, "Client connected on FD %d\n", socket);
    Message received;
    char payload[256];
    char line[256];
    Message info;
    info.type = Info::TYPE;
    //expect a sit message
    if (reader.readline(line, 255)  < 0) { //bad input
        logger->log(log_error, "Unable to read data from socket");
        close(socket);
        return nullptr;
    }
    logger->log(log_debug, "Received from client: %s", line);
    if (Message::parse_message(line, received) && received.code == Command::INCOMING) {
        logger->log(log_debug, "Client accepted");
        if (shared_chairs->chair >= shared_chairs->freeSpace) {
            Message bye(Answer::TYPE, Answer::NO_SPACE, Answer::S_NO_SPACE);
            bye.to_string(payload);
//            logger->log(log_plain, "Sending %s", payload);
            bye.send(socket);
            close(socket);
            return nullptr;
        } else {
            logger->log(log_debug, "Welcoming client");
            testvalue(sem_wait(sem_lock), -1, *logger, "Unable to enter critical section");
            sprintf(payload, Answer::S_SIT, shared_chairs->chair++);
            testvalue(sem_post(sem_lock), -1, *logger, "Unable to leave critical section");
            Message welcome(Answer::TYPE, Answer::SIT, payload);
            logger->log(log_plain, "Sending %s", payload);
            testvalue(welcome.send(socket), -1, *logger, "Unable to write");
        }
        do {
            if (rand() % 101 > 80) {
                Info::random(rand() % Info::count + 1, info.code, info.text);
                logger->log(log_plain, "Sending info message I%d:%s", info.code, info.text);
                info.send(socket);
                memset(info.text, '\0', sizeof(char) * 251);
            }
            logger->log(log_debug, "Waiting for client!");
            //expect a message
            if (reader.readline(line, 255)  < 0) { //bad input
                logger->log(log_error, "Unable to read data from socket");
                break;
            }
            //is hungry
            if (Message::parse_message(line, received) && received.code == Command::HUNGRY) {
                logger->log(log_debug, "Trying to get forks");
                testvalue(sem_wait(sem_forks), -1, *logger, "Unable decrease FORKS semaphore");
                testvalue(sem_wait(sem_forks), -1, *logger, "Unable decrease FORKS semaphore");
                sprintf(payload, Answer::S_EAT);
                Message eat(Answer::TYPE, Answer::EAT, payload);
                logger->log(log_debug, "Sending: %s", payload);
                eat.send(socket);
            } //is leaving
            else if (Message::parse_message(line, received) && received.code == Command::LEAVING) {
                logger->log(log_info, "Client leaving");
                testvalue(sem_wait(sem_lock), -1, *logger, "Unable to enter critical section");
                sprintf(payload, Answer::S_BYE);
                shared_chairs->chair--;
                testvalue(sem_post(sem_lock), -1, *logger, "Unable to leave critical section");
                Message bye(Answer::TYPE, Answer::BYE, payload);
                logger->log(log_plain, "Sending %s", payload);
                testvalue(bye.send(socket), -1, *logger, "Unable to write");
                break;
            } else { //else is invalid
                Message invalid = Message(Error::TYPE, Error::CLIENT_ERROR, Error::S_CLIENT_ERROR);
                invalid.send(socket);
                break;
            }
            //expect a message
            if (rand() % 101 > 80) {
                Info::random(rand() % Info::count + 1, info.code, info.text);
                logger->log(log_plain, "Sending info message I%d:%s", info.code, info.text);
                info.send(socket);
                memset(info.text, '\0', sizeof(char) * 251);
            }
            if (reader.readline(line, 255)  < 0) { //bad input
                logger->log(log_error, "Unable to read data from socket");
                break;
            }
            if (Message::parse_message(line, received) && received.code == Command::FULL) {
                testvalue(sem_post(sem_forks), -1, *logger, "Unable increase FORKS semaphore");
                testvalue(sem_post(sem_forks), -1, *logger, "Unable increase FORKS semaphore");
                sprintf(payload, Answer::S_FORK);
                Message fork(Answer::TYPE, Answer::FORK, payload);
                logger->log(log_debug, "Sending: %s", payload);
                fork.send(socket);
            } else {
                Message invalid = Message(Error::TYPE, Error::CLIENT_ERROR, Error::S_CLIENT_ERROR);
                invalid.send(socket);
                break;
            }
        } while (1);
    } else {
        logger->log(log_debug, "Client error");
        Message invalid = Message(Error::TYPE, Error::CLIENT_ERROR, Error::S_CLIENT_ERROR);
        invalid.send(socket);
    }
    close(socket);
}

