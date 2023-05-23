#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#define TAPE_SIZE 10

typedef struct stack {
    int size;
    int *contents;
} stack;

void die(char *e, int l) {
    if (l != -1) {
        printf("%s at %d.\n", e, l + 1);
    } else {
        printf("%s\n", e);
    }

    exit(1);
}

void pop(stack *s) {
    s->size -= 1;
}

char* read_file(char *file_name) {
    FILE* f;
    if ((f = fopen(file_name, "r")) == NULL) die("Cannot open file.", -1);
    
   int rc = fseek(f, 0L, SEEK_END);
   long offset_end = ftell(f);
   size_t fs = (size_t)offset_end;

    rewind(f);
    char *buffer = malloc(fs);
    char *p = malloc(fs);
    fread(buffer, 1, fs, f);

    int p_length = 0;
    for (int i = 0; i < strlen(buffer); i++) {
        char c = buffer[i];
        if (c == '>' || c == '<' || c == '+' || c == '-' || c == '[' || c == ']' || c == '.' || c == ',') {
            p[p_length] = c;
            p_length++;
        }
    }

    free(buffer);
    return p;
}

void init_tape(int *tape) {
    for (int i = 0; i < TAPE_SIZE; i++) {
        tape[i] = 0;
    }
}

int* compile(char *program) {
    int *tape = malloc(sizeof(int) * TAPE_SIZE);
    init_tape(tape);

    int ptr = 0;

    bool looping = false;
    int inner_loops = 0;
    
    stack loop_stack = {0, malloc(sizeof(char) * strlen(program))};

    for (int i = 0; i < strlen(program); i++) {
        char c = program[i];

        if (looping) {
            if (c == '[') inner_loops++;

            if (c == ']') {
                if (inner_loops == 0)  {
                    looping = false;
                } else {
                    inner_loops--;
                }
            }

            continue;
        }

        switch (c) {
            case '+':
                tape[ptr]++;
                break;
            case '-':
                tape[ptr]--;
                break;
            case '>':
                if (ptr + 1 >= TAPE_SIZE) die("Moved beyond tape boundaries", i);
                ptr++;
                if (!tape[ptr]) tape[ptr] = 0;
                break;
            case '<':
                if (ptr - 1 < 0) die("Moved beyond tape boundaries", i);
                ptr--;
                if (!tape[ptr]) tape[ptr] = 0;
                break;
            case '.':
                printf("%c", tape[ptr]);
                break;
            case ',':
                scanf("%c", &tape[ptr]);
                break;
            case '[':
                if (tape[ptr] == 0) {
                    looping = true;
                } else {
                    loop_stack.contents[loop_stack.size] = i;
                    loop_stack.size++;
                }
                break;
            case ']':
                if (tape[ptr] != 0) {
                    i = loop_stack.contents[loop_stack.size - 1];
                } else {
                    loop_stack.size--;
                }
                break;
            default:
                break;
        }
    }

    free(loop_stack.contents);
    return tape;
}

int main(int argc, char *argv[]) {
    char *input = read_file(argv[1]);
    int *tape = compile(input);
    
    free(input);
    printf("\n");
    free(tape);
}