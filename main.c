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

void throw_error(enum err_t err);
FILE *open_file(const char *path, const char *mode); 
void calc(FILE *f);
void print_help(); 
bool parse_flags(const char *flags);

int main(int argc, char *argv[]) {
    int i = 1, j;
    bool is_was_flg = false;

    if (argc < 2) {
        throw_error(FEWARGC);
        exit(1);
    } 
    for (j = i; j < argc-1; ++j) {
        is_was_flg = parse_flags(argv[i]);
        if (results.is_p_help) {
            print_help();
            return 0;
        }
    }

    if (is_was_flg) {
        i = j-1;
    }
    
    
    unsigned long long sum_lc, sum_wc, sum_cc;
    sum_lc = sum_wc = sum_cc = 0;
    while (i < argc) {
        FILE *f = open_file(argv[i], "r");
    
        calc(f);

        sum_lc += results.lc;
        sum_wc += results.wc;
        sum_cc += results.cc;

        fclose(f);

        if (results.is_p_line)
            printf("%llu ", results.lc);
        if (results.is_p_word)
            printf("%llu ", results.wc);
        if (results.is_p_character)
            printf("%llu ", results.cc);
        printf("%s\n", argv[i]);
        ++i;
    }

    if (results.is_p_line)
        printf("%llu ", sum_lc);
    if (results.is_p_word)
        printf("%llu ", sum_wc);
    if (results.is_p_character)
        printf("%llu ", sum_cc);
    printf("Total\n");

    return 0;
}


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
    printf("Type wmc <flags> <path to file> for to run with custom settings\n");
    printf("Possible flags: \n\t'-l' - to count the rows\n\t'-w' - to count the words\n\t'-c' - to count the characters\n\t'-h' - to print help\n");
}

bool parse_flags(const char *flags) {
    size_t len = strlen(flags);
    if (flags[0] != '-' || len < 2) {
        results.is_p_line = results.is_p_word = results.is_p_character = true;
        results.is_p_help = false;
        return false;
    }

    results.is_p_help = results.is_p_line = results.is_p_word = results.is_p_character = false;
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
    return true;
}
