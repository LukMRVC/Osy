//
// Created by Lukas on 02.04.2020.
//

#ifndef READLINE_BUFFEREDFILEDESCRIPTORREADER_H
#define READLINE_BUFFEREDFILEDESCRIPTORREADER_H

#include "FileDescriptorReader.h"

class BufferedFileDescriptorReader : public FileDescriptorReader {
public:
    explicit BufferedFileDescriptorReader(const int fd, const int buffer_size = 1024);

    int read(char *buffer, int maxLen) override;

    int readline(char *line, int maxLen) override;

    ~BufferedFileDescriptorReader();

private:
    char *buffer = nullptr;
    int buffer_size;
    int buffer_pos = 0;
    int read_bytes = 0;
};

BufferedFileDescriptorReader::BufferedFileDescriptorReader(const int fd, const int buffer_size)
        : FileDescriptorReader(fd) {
    this->buffer_size = buffer_size;
    buffer = new char[buffer_size]();
}

int BufferedFileDescriptorReader::read(char *buf, int maxLen) {
    int copied = 0;
    int to_copy = 0;
    if (maxLen >= buffer_size) {
        return ::read(this->file_descriptor, buffer, maxLen);
    }

    if (buffer_pos + maxLen >= buffer_size) {
        memcpy(buf, this->buffer + buffer_pos, buffer_size - buffer_pos);
        copied = buffer_size - buffer_pos;
        buffer[buffer_pos] = '\0';
    }

    if (buffer[buffer_pos] == '\0') {
        read_bytes = ::read(this->file_descriptor, this->buffer, this->buffer_size);
        if (read_bytes <= 0)
            return read_bytes;
        buffer_pos = 0;
    }
    to_copy = maxLen - copied;
    if (read_bytes < to_copy) {
        to_copy = read_bytes;
    }
    memcpy(buf + copied, buffer + buffer_pos, to_copy);
    buffer_pos += to_copy;
    return maxLen;
}


BufferedFileDescriptorReader::~BufferedFileDescriptorReader() {
    if (buffer != nullptr) {
        delete[] buffer;
        buffer = nullptr;
    }
}

int BufferedFileDescriptorReader::readline(char *line, int maxLen) {
    int line_len = 0;
    int str_offset = 0;
    int copied = 0;

    while (line_len < maxLen) {

        if (buffer[buffer_pos + str_offset] == '\0' || (buffer_pos + str_offset) >= read_bytes
            || (buffer_pos + str_offset) >= buffer_size) {
            memcpy(line + copied, buffer + buffer_pos, str_offset);
            read_bytes = ::read(this->file_descriptor, this->buffer, this->buffer_size);
            if (read_bytes <= 0) {
                return read_bytes;
            }
            buffer_pos = 0;
            copied += str_offset;
            str_offset = 0;
        }

        if (buffer[buffer_pos + str_offset] == '\n') {
            str_offset++; //read with newline
            line_len++;
            break;
        }

        str_offset++;
        line_len++;
    }

    memcpy(line + copied, buffer + buffer_pos, str_offset);
    buffer_pos += str_offset;
    *(line + copied + str_offset) = 0;
    return line_len;
}

#endif //READLINE_BUFFEREDFILEDESCRIPTORREADER_H
