#include "kernel/types.h"
#include "user/user.h"

int main()
{
    char byte = 'a';
    int p_c_to_p[2];
    int p_p_to_c[2];
    int pid;

    // create two pipes for mutual communication
    if (pipe(p_c_to_p) && pipe(p_p_to_c) != 0) {
        fprintf(1, "Error when creating pipes");
        exit(1);
    }

    if (fork() == 0) {
        close(p_c_to_p[0]);
        close(p_p_to_c[1]);
        read(p_p_to_c[0], &byte, 1);
        pid = getpid();
        printf("%d: received ping\n", pid);
        write(p_c_to_p[1], &byte, 1);
        close(p_c_to_p[1]);
        close(p_p_to_c[0]);
    } else {
        close(p_c_to_p[1]);
        close(p_p_to_c[0]);
        write(p_p_to_c[1], &byte, 1);
        pid = getpid();
        read(p_c_to_p[0], &byte, 1);
        printf("%d: received pong\n", pid);
        close(p_c_to_p[0]);
        close(p_p_to_c[1]);
    }
    exit(0);
}