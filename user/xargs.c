#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

int readline(char** new_args, int i) {
    char ch;
    char buffer[512];
    char* p = buffer;
    char* word;

    while(read(0, &ch, 1)) {
        if (ch == '\n')
            break;
        if (p == buffer+512) {
            fprintf(2, "xargs: too long a line");
            exit(1);
        }
        *p = ch;
        p++;
    }
    *p = '\0';

    p = word = buffer;
    while (1) {
        // printf("%s\n",word);
        if (*p == ' ') {
            *p = '\0';
            if (strlen(word) == 0) continue;
            new_args[i] = malloc(strlen(word) + 1);
            strcpy(new_args[i++], word);
            word = p+1;
            p++;
        } else if (*p == '\0') {
            if (strlen(word) == 0) break;
            new_args[i] = malloc(strlen(word) + 1);
            strcpy(new_args[i++], word);
            break;
        } else {
            p++;
        }
    }
    new_args[i] = '\0';
    return i;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(2, "Usage: xargs <command>");
        exit(1);
    }

    char* new_argv[MAXARG];
    char* command = malloc(strlen(argv[1]+1));
    int i = 1;

    strcpy(command, argv[1]);
    for (i = 0; i < argc-1; ++i) {
        new_argv[i] = malloc(strlen(argv[i+1]) + 1);
        strcpy(new_argv[i], argv[i+1]);
        // printf("test: %s\n", new_argv[i]);    
    }

    int cur_argc = i;
    while (readline(new_argv, cur_argc) != cur_argc) { 
        if (fork() == 0) {
            exec(command, new_argv);
        }
        wait(0);
    } 

    exit(0);
}