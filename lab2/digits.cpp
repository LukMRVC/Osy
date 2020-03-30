#include <cstdio>
#include <ctime>
#include <string>
#include <cstdlib>

int main(int argc, char** argv) {
    srand(time(nullptr));

    int rowCount = std::atoi(argv[1]);

    for (int j = 0; j < rowCount; ++j) {
        int sum = 0;
        for (int i = 0; i < rand() % 15 + 2; ++i) {
            int _rand = rand() % 20;
            printf("%d ", _rand);
            sum += _rand;
        }
        printf("%d\n", sum);
    }
    
    return 0;
}