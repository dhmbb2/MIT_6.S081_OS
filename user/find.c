#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void _find(char* path, char* file_name) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type) {
    case T_FILE:
        fprintf(2, "Usage: find <dir> <file>");
        break;
    case T_DIR:
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
            printf("find: path too long\n");
        strcpy(buf, path);
        p = buf + strlen(path);
        *p++ = '/';
        while(read(fd, &de, sizeof(de)) == sizeof(de)) {
            // don't recurse into "." and ".."
            if(de.inum == 0 || strcmp(de.name, ".")==0 || strcmp(de.name, "..")==0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if(stat(buf, &st) < 0){
                printf("find: cannot stat %s\n", buf);
                continue;
            }
            if (st.type==T_FILE && strcmp(de.name, file_name) == 0)
                printf("%s\n", buf);
            if (st.type==T_DIR)
                _find(buf, file_name);
        }
        break;
    }
    close(fd);
}

int main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(2, "Usage: find <dir> <file>");
        exit(1);
    }    
    _find(argv[1], argv[2]);
    exit(0);
}