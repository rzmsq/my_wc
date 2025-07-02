#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

struct {
    unsigned long long lc;
    unsigned long long wc;
    unsigned long long cc;
    bool is_p_help;
    bool is_p_line;
    bool is_p_word;
    bool is_p_character;
} results;

enum err_t {
    FEWARGC,
    EFILE,
    EFLG,
};

enum args_name {
    path = 1,
    flags,
};

void throw_error(enum err_t err) {
    if (err == FEWARGC)
        fprintf(stderr, "[ERROR] Few arguments type mwc -h for help\n");
    else if (err == EFILE)
        fprintf(stderr, "[ERROR] File not open\n");
    else if (err == EFLG)
        fprintf(stderr, "[ERROR] Incorrect flags type mwc -h for help\n");
}

FILE *open_file(const char *path, const char *mode) {
    FILE *f;
    if ((f = fopen(path, mode)) == NULL) {
        throw_error(EFILE);
        exit(1);
    }
    return f;
}

void calc(FILE *f) {
    results.wc = results.lc = results.cc = 0;
    int ch;
    bool is_in_word = false;
    while ((ch = getc(f)) != EOF) {
        results.cc++;
        if (isspace(ch)) {
            if (ch == '\n')
                results.lc++;
            is_in_word = false;
            continue;
        }
        if (!is_in_word){
            is_in_word = true;
            results.wc++;
        }
    }
}

void print_help() {
    printf("Type wmc <path to file> for to run with default settings\n");
    printf("Type wmc <path to file> <flags> for to run with custom settings\n");
    printf("Possible flags: \n\t'-l' - to count the rows\n\t'-w' - to count the words\n\t'-c' - to count the characters\n\t'-h' - to print help\n");
}

void parse_flags(const char *flags) {
    size_t len = strlen(flags);
    if (flags[0] != '-' || len < 2) {
        throw_error(EFLG);
        exit(1);
    }
    for (size_t i = 1; i < len; ++i) {
        if (flags[i] == 'h') {
            results.is_p_help = true;
            break;
        }
        else if (flags[i] == 'w') {
            results.is_p_word = true;
        } else if (flags[i] == 'l') {
            results.is_p_line = true;
        } else if (flags[i] == 'c') {
            results.is_p_character = true;
        } else {
            throw_error(EFLG);
            exit(1);
        }
    }
}


int main(int argc, char *argv[]) {

    if (argc < 2) {
        throw_error(FEWARGC);
        exit(1);
    } else if (argc > 2) {
        results.is_p_help = results.is_p_line = results.is_p_word = results.is_p_character = false;
        for (int i = flags; i < flags+argc-2; ++i) {
            parse_flags(argv[i]);
            if (results.is_p_help) {
                print_help();
                return 0;
            }
        }
    } else {
        results.is_p_line = results.is_p_word = results.is_p_character = true;
    }

    FILE *f = open_file(argv[path], "r");
    
    calc(f);

    fclose(f);

    if (results.is_p_line)
        printf("%llu ", results.lc);
    if (results.is_p_word)
        printf("%llu ", results.wc);
    if (results.is_p_character)
        printf("%llu ", results.cc);
    printf("%s\n", argv[path]);

    return 0;
}
