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
 *    a. does require pruning for node_modules libraries and .files, etc.
 * 3. Testing and Validation
 * 4. Auditing
 *
      type          file type
      path          full path
      sb.st_ino     inode number
      sb.st_mode    file mode
      sb.st_nlink   number of links
      sb.st_uid     user id of owner
      sb.st_gid     group id of owner
      sb.st_blksize filesystem preferred block size
      sb.st_size    file size in bytes
      sb.st_blocks  number of 512 byte blocks allocated for this file (linux)
      sb.st_ctime   creation time
      sb.st_atime   access time
      sb.st_mtime   modification time
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
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
            continue;
        }
        char path[BUFSIZ];
        
        strcpy(path, name);
        if (strcmp(name, "/") != 0) strcat(path, "/");
        strcat(path, dp->d_name);

        char type[4] = "UNKN";
        struct stat sb;
        lstat(path, &sb);
        switch (sb.st_mode & S_IFMT) {
        case S_IFDIR:
            if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
                break;
            }
            if (strcmp(name, "/dev/fd") == 0) break;
            printf("FDIR|%s|%ld|%lo|%ld|%ld|%ld|%ld|%lld|%lld|%ld|%ld|%ld\n",
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
            if (type[0] == 'U') strcpy(type, "FCHR");
        case S_IFBLK:
            if (type[0] == 'U') strcpy(type, "FBLK");
        case S_IFIFO:
            if (type[0] == 'U') strcpy(type, "FIFO");
        case S_IFLNK:
            if (type[0] == 'U') strcpy(type, "FLNK");
        case S_IFSOCK:
            if (type[0] == 'U') strcpy(type, "SOCK");
        case S_IFREG:
            if (type[0] == 'U') strcpy(type, "FREG");
            printf("%s|%s|%ld|%lo|%ld|%ld|%ld|%ld|%lld|%lld|%ld|%ld|%ld\n",
                type,
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

