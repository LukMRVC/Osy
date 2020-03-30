#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <string>

int eval(int a, char op, int b) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
        case '%': return a % b;
    }
}

int main(int argc, char ** argv ) {
    srand(time(NULL));
    if (argc == 1) {
        exit(-1);
    }
    int count = std::stoi(argv[1]);

    char operands[] = { '+', '-', '*', '/', '%' };

    for (int i = 0; i < count; ++i) {
        char op = operands[rand() % 5];
        int operandA = rand() % 100;
        int operandB = (rand() % 100) + 1;
        printf("%d%c%d=%d\n", operandA, op, operandB, eval(operandA, op, operandB));
    }
	
    return 0;

}

