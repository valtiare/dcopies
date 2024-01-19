#include <stdio.h>

enum instructionSet {
    PSH,
    ADD,
    POP,
    HLT
};

int running = 1;
int ip = 0;
int sp = -1;
int stack[256];

const int program[] = {
    PSH, 5,
    PSH, 6,
    ADD,
    POP,
    HLT
};

int fetch() {
    return program[ip];
}

void eval(int instruction) {
    switch (instruction) {
        case HLT:
            running = 0;
            printf("[debug] done.\n");
            break;

        case PSH:
            sp++;
            stack[sp] = program[++ip];
            break;

        case POP:
            printf("popped %d\n", stack[sp--]);
            break;

        case ADD: {
            int a = stack[sp--];
            int b = stack[sp--];
            int result = b + a;
            sp++;
            stack[sp] = result;
            break;
        }

        default:
            break;
    }
}

int main() {
    while (running) {
        eval(fetch());
        ip++;
    }

    return 0;
}
