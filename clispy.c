#include <stdio.h>
#include <stdlib.h>
#include <clispy.h>

char **tokenize(size_t n, char *s) {
}

struct ParseTree *parse(size_t n, char *program) {
    struct ParseTree *pt = malloc(sizeof(struct ParseTree));
    return pt;
}

int main(int args, char *argv[]) {
    FILE *f = fopen(argv[1], "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    rewind(f);

    char *program = malloc(fsize + 1);
    fread(program, 1, fsize, f);
    fclose(f);

    program[fsize] = 0;
}
