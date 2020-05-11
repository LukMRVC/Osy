//
// Created by Lukas on 11.05.2020.
//

#ifndef OSY_PROJECT_PROGRAMARGS_H
#define OSY_PROJECT_PROGRAMARGS_H
#include "Logger.h"

#include <stdlib.h>

struct ProgramArgs {
    static constexpr int client = 2;
    static constexpr int server = 1;
    unsigned int log_level = log_info;
    unsigned int port = 0;
    char *host = nullptr;

    static ProgramArgs parse(int argc, char ** argv, int type) {
        if (argc <= type)
            help(argc, argv, type);
        ProgramArgs args;
        for (int i = 1; i < argc; ++i) {
            if (!strcasecmp(argv[i], "-d"))
                args.log_level = log_debug;
            if (!strcasecmp(argv[i], "-h"))
                help(argc, argv, type);
            if (type == server) {
                if (*argv[i] != '-' && !args.host) {
                    args.port = atoi(argv[i]);
                    return args;
                }
            } else {
                if (*argv[i] != '-') {
                    if (args.host == nullptr)
                        args.host = argv[i];
                    else if (!args.port) {
                        args.port = atoi(argv[i]);
                    }
                }
            }
        }
        if (type == server && !args.port)
            Logger::print(log_error, "No port specified, see help -h");
        if (type == client && (args.host == nullptr || !args.port))
            Logger::print(log_error, "No port or hostname specified, see help -h");
        return args;
    }

    static void help(int argc, char ** argv, int type) {
        if (argc <= type) {
            errno = EINVAL;
            Logger::print(log_error, "Insufficient program arguments supplied, see help -h");
            exit(0);
        }

        if (type == client) {
            Logger::print(log_plain, "\n"
                                    " Usage: %s [-h -d] host port_number \n"
                                    " -h  help \n"
                                    " -d  debug mode \n"
                                    "\n ", argv[0]);
        }

        if (type == server) {
            Logger::print(log_plain, "\n"
                                    " Usage: %s [-h -d] port_number \n"
                                    " -h  help \n"
                                    " -d  debug mode \n"
                                    "\n ", argv[0]);
        }
        exit( 0 );
    }
};

#endif //OSY_PROJECT_PROGRAMARGS_H
