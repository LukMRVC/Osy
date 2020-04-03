//
// Created by Lukas on 02.04.2020.
//

#ifndef READLINE_FILEDESCRIPTORREADER_H
#define READLINE_FILEDESCRIPTORREADER_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


class FileDescriptorReader {
public:
    explicit FileDescriptorReader(const int fd);

    virtual int read(char *buffer, int maxLen);

    virtual int readline(char *line, int maxLen);

protected:
    int file_descriptor;
};

FileDescriptorReader::FileDescriptorReader(const int fd) {
    file_descriptor = fd;
}

int FileDescriptorReader::read(char *buffer, int maxLen) {
    return ::read(file_descriptor, buffer, maxLen);
}

int FileDescriptorReader::readline(char *line, int maxLen) {
    int line_len = 0;
    int read_bytes;
    do {
        read_bytes = ::read(this->file_descriptor, line + line_len, 1);
        line_len++;
        if (read_bytes <= 0) {
            return read_bytes;
        }
    } while (line[line_len - 1] != '\n' && line_len < maxLen);

    line[line_len] = 0;
    return line_len;
}


#endif //READLINE_FILEDESCRIPTORREADER_H
