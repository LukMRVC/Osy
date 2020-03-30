#include <cstdio>
#include <string>
#include <iostream>

int main(void) {

    int sum = 0;
    int num_pos;
    int num;
    int r;
    std::string line;
    const char *c_line;
    while (std::getline(std::cin, line)) {
        c_line = line.c_str();

        while(sscanf(c_line, "%d", &num) == 1) {
            printf("%d", num);
        }
        printf("\n");

    }



    return 0;
}