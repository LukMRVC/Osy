//
// Created by Lukas on 11.05.2020.
//
#ifndef OSY_STRUCT_MESSAGE_H
#define OSY_STRUCT_MESSAGE_H

#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include "Logger.h"

struct  Message {
    char type;
    int code;
    char * text = nullptr;
    Message() {
        text = new char [251];
    }

    Message(char t, int c, const char * txt) {
        type = t;
        code = c;
        text = new char[251];
        strcpy(text, txt);
    }

    ~Message() {
        if (text != nullptr) {
            delete [] text;
        }
        text = nullptr;
    }

    static bool parse_message(const char * string_message, Message & mes) {
        switch (string_message[0]) {
            case 'A':
            case 'C':
            case 'W':
            case 'E':
            case 'I':
                mes.type = string_message[0];
                break;
            default: return false;
        }
        if (string_message[1] == ':') {
            mes.code = -1;
            strcpy(mes.text, (string_message + 2));
        } else {
            int counter = 0;
            while (string_message[1 + counter] != ':') {
                if (string_message[ 1 + counter ] >= '0' && string_message[ 1 + counter ] <= '9' && counter < 2)
                    counter++;
                else
                    return false;
            }
            mes.code = ((int)(string_message[1] - '0')) * 10 + ((int)(string_message[2] - '0'));
            strcpy(mes.text, (string_message + 4));
        }
        return true;
    }

    void to_string(char * string, bool include_newline = true) {
        if (include_newline)
            sprintf(string, "%c%d:%s\n", type, code, text);
        else
            sprintf(string, "%c%d:%s", type, code, text);
    }

    int send(int fd, ...)
    {
        char str[256];
        to_string(str);
        va_list arg;
        va_start( arg, fd );
        vsprintf( str, str, arg );
        va_end( arg );
        Logger::print(log_debug, "Writing %s to FD: %d", str, fd);
        return write(fd, str, strlen(str));
    }

};

#endif