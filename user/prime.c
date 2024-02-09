#include "kernel/types.h"
#include "user/user.h"

void sift_k(int k, int p0[]) {
    int cur_num = 0;
    int flag = 0; // whether this proc has met the first undividable num or not
    close(p0[1]);
    int read_fd = p0[0];

    int p[2];
    pipe(p);
    int write_fd = p[1];

    printf("prime %d\n", k);

    // read from the left process;
    while(read(read_fd, &cur_num, sizeof(int))) {
        if (cur_num % k == 0){
            continue;
        }

        if (!flag) {
            flag = 1;
            if (fork() == 0) sift_k(cur_num, p);
            close(p[0]); // pipe connecting this process and its right neighbor
        } else {
            write(write_fd, &cur_num, sizeof(int));
        }
    }
    close(read_fd);
    close(write_fd);
    if (flag)
        wait(0);

    exit(0);
}

int main() {
    int p[2];
    pipe(p);

    if (fork() == 0) {
        sift_k(2, p);
    }
    
    close(p[0]);
    for (int i = 3; i < 36; ++i) {
        write(p[1], &i, sizeof(int));
    }
    
    close(p[1]);
    wait(0);

    exit(0);
}