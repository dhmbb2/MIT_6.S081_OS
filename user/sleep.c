#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(2, "Usage of sleep: sleep <time/seconds>");
        exit(1);
    }
    int tick = atoi(argv[1]);
    sleep(tick);
    fprintf(1, "(nothing happen for a little while)\n");
    exit(0);
}