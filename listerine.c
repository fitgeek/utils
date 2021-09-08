/*
Copyright 2021 Richard 'Rick' Hernandez 619-847-3241

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
/*
 * file listing utility - written in C for speed
 * use cases
 * 1. archive file state 
 *    a. provides a static picture prior to file system changes
 *    b. diff between old and new file system state
 * 2. diff between systems to determine out of sync issues.
 *    a. does require pruning for node_modules libraries and .files
 */
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

void listit(char *name) {
    DIR *dir;
    struct dirent *dp;

    if (!(dir = opendir(name))) {
        return;
    }

    while((dp = readdir(dir)) != NULL) {
        char path[BUFSIZ];
        strcpy(path, name);
        strcat(path, "/");
        strcat(path, dp->d_name);

        struct stat sb;
        stat(path, &sb);
        switch (sb.st_mode & S_IFMT) {
        case S_IFDIR:
            if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
                break;
            }
            printf("%s|%ld|%lo|%ld|%ld|%ld|%ld|%lld|%lld|%ld|%ld|%ld\n",
                path,
                (long) sb.st_ino,
                (unsigned long) sb.st_mode,
                (long) sb.st_nlink,
                (long) sb.st_uid,
                (long) sb.st_gid,
                (long) sb.st_blksize,
                (long long) sb.st_size,
                (long long) sb.st_blocks,
                sb.st_ctime,
                sb.st_atime,
                sb.st_mtime);
            listit(path);
            break;
        case S_IFCHR:
        case S_IFBLK:
        case S_IFIFO:
        case S_IFLNK:
        case S_IFSOCK:
        case S_IFREG:
            printf("%s|%ld|%lo|%ld|%ld|%ld|%ld|%lld|%lld|%ld|%ld|%ld\n",
                path,
                (long) sb.st_ino,
                (unsigned long) sb.st_mode,
                (long) sb.st_nlink,
                (long) sb.st_uid,
                (long) sb.st_gid,
                (long) sb.st_blksize,
                (long long) sb.st_size,
                (long long) sb.st_blocks,
                sb.st_ctime,
                sb.st_atime,
                sb.st_mtime);
            break;
        default:
            printf("Error unknown file type: %s\n", path);
        }
    }
    closedir(dir);
}

int main(int argc, char **argv) {
    listit(argv[1]);
}

