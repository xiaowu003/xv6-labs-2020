#include "kernel/types.h"

#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

void find(char *path, const char *filename);

int main(int argc, char *argv[]) {

    if (argc == 2) {
        find(".", argv[1]);
    } else if (argc == 3) {
        find(argv[1], argv[2]);
    } else {
        fprintf(2, "usage: find <directory> <filename>\n");
        exit(1);
    }

    exit(0);
}

void find(char *path, const char *name) {
    char buf[512], *p;
    int fd;
    struct dirent de;   //文件或文件夹属性
    struct stat st;     //获取目标的属性

    if ( (fd = open(path, 0)) < 0 ) {
        fprintf(2, "find: cannot open %s\n", path);
        exit(1);
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot fstat %s\n", path);
        close(fd);
        exit(1);
    }

    if (st.type != T_DIR) {
        fprintf(2, "Usage: find <DIRECTORY> <filename>\n");
        close(fd);
        exit(1);
    }

    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0) {
            continue;
        }

        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = '\0';

        if ( stat(buf, &st) < 0) {
            fprintf(2, "find: cannot fstat %s\n", buf);
            continue;
        }

        if (st.type == T_DIR && strcmp( p, "." ) != 0 && strcmp( p, ".." ) != 0) {
            find(buf, name);
        } else if (strcmp(p, name) == 0) {
            printf("%s\n", buf);
        }

    }

    close(fd);
}
