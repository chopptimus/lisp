#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "clispy.h"

Tokens tokenize(size_t n, char *s) {
    char **tokens = malloc(sizeof(char *) * n);
    int j = 0;

    int start = 0;
    int len;

    char *tok;
    for (int i = 0; i < n; i++) {
        if (s[i] == '(' || s[i] == ')') {
            tok = malloc(sizeof(char *) * 2);
            tok[0] = s[i];
            tok[1] = 0;
            tokens[j] = tok;
            j++;
            start = i + 1;
        } else if (s[i] == ' ') {
            len = i - start;
            tok = malloc(sizeof(char) * len + 1);
            strncpy(tok, &s[start], len);
            tok[len] = 0;
            tokens[j] = tok;
            j++;
            start = i + 1;
        }
    }

    return (Tokens) { j, tokens };
}

ParseTree parse(size_t n, char *program) {
    Tokens tokens = tokenize(n, program);
    printf("%zu\n", tokens.size);
    for (int i = 0; i < tokens.size; i++) {
        printf("%s\n", tokens.tokens[i]);
    }
    struct ParseTree pt = { Number, NULL };
    return pt;
}

int main(int args, char *argv[]) {
    FILE *f = fopen(argv[1], "rb"); // Error handling?
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    rewind(f);

    char *program = malloc(fsize + 1);
    fread(program, 1, fsize, f);
    fclose(f);

    program[fsize] = 0;

    ParseTree pt = parse(fsize, program);
    return pt.type;
}
