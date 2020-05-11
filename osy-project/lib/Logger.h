//
// Created by Lukas on 11.05.2020.
//

#ifndef OSY_PROJECT_LOGGER_H
#define OSY_PROJECT_LOGGER_H
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <cstring>

constexpr unsigned int log_info = 0;
constexpr unsigned int log_debug = 1;
constexpr unsigned int log_error = 2;
constexpr unsigned int log_plain = 3;

struct Logger {
    int descriptor;
    Logger(int file_no) {
        descriptor = file_no;
    }

    constexpr static const char * format[] = {
            "INF: %s\n",
            "DEB: %s\n",
            "ERR: (%d-%s) %s\n",
            "%s\n",
    };

    void log(unsigned int level, const char * message, ...) {
        if (level && level > log_plain) return;
        char buf[ 1024 ];
        va_list arg;
        va_start( arg, message );
        vsprintf( buf, message, arg );
        va_end( arg );
        switch (level) {
            case log_info:
            case log_debug:
                dprintf(descriptor, format[ level ], buf);
                break;
            case log_error:
                dprintf(descriptor, format[ level ], errno, strerror(errno), buf);
                break;
            case log_plain:
                dprintf(descriptor, format[ level ], buf);
                break;
        }

    }

    static void print(unsigned int level, const char * message, ...) {
        if (level && level > log_plain) return;
        char buf[ 1024 ];
        va_list arg;
        va_start( arg, message );
        vsprintf( buf, message, arg );
        va_end( arg );
        switch (level) {
            case log_info:
            case log_debug:
                fprintf(stdout, format[ level ], buf);
                break;
            case log_error:
                fprintf(stdout, format[ level ], errno, strerror(errno), buf);
                break;
            case log_plain:
                fprintf(stdout, format[ level ], buf);
                break;
        }

    }

};

#endif //OSY_PROJECT_LOGGER_H
