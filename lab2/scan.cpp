#include <string>
#include <cstdio>
#include <iostream>

int main(int argc, char ** argv) {
    
    int sum = 0;
    int num_pos;
    int num;
    std::string line;
    const char *c_line;
    // size_t line;
    int i = 1;
    while (std::getline(std::cin, line)) {
        num_pos = 0;
        sum = 0;
        c_line = line.c_str();
        while( sscanf(c_line, "%d%n", &num, &num_pos) == 1) {
            c_line += num_pos;
            sum += num;
        }
        if (sum - num == num) {
            if (argc == 2) {
                if (argv[argc - 1] == 'e') {
                    write(2, sum - num);
                } 
            }

            printf("Radek %d je validni.\n", i);
            printf("Posledni cislo je %d, suma bez posledniho cisla %d\n", num, sum - num);
        } else {
            printf("Radek %d neni validni.: %d %d\n", i, sum, num);
        }
        ++i;
    
    }
    //ssh mor0179@linedu /home/fei/mor0179/Documents/osy/digits 5 | ./scan
    //ssh mor0179@linedu "/home/fei/mor0179/Documents/osy/digits 5 | gzip -" | gunzip - | ./scan


    return 0;
}