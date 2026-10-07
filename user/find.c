#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"

int exec_command (char *path, char *argv[]){
    char *new_argv[MAXARG];
    memset(new_argv, 0, sizeof(new_argv));

    int i = 0;

    while (argv[4+i] != 0 && i < MAXARG - 2) {
        new_argv[i] = argv[4 + i];
        i++;
    }
    new_argv[i] = path;
    new_argv[i + 1] = 0;

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "find : fork failed\n");
        return -1;
    } else if (pid == 0) {
        exec(new_argv[0], new_argv);
        fprintf(2, "execution %s failed\n", new_argv[0]);
        exit(1);
    } 
    wait(0);
    return 0;
}

void find(char *arg1, char *argv[], int isExec) {
    char *path = arg1;
    char *search_name = argv[2];

    char buf[512], *p;
    int fd;
    struct dirent de; // predefined in file system; short for directory entry
    struct stat st; // also predefined, stores information about a file in fs

    if ((fd = open(path, O_RDONLY)) < 0) { // opening a directory that is given path
    fprintf(2, "find: cannot open %s\n", path); // open fails
    return;
    }
    // if we are here we have successfully open the directory(of path)
  
    if (fstat(fd, &st) < 0) { // fstat takes out information of certain directory
        // and saves it in the stuct stat that is here st.
        fprintf(2, "find: cannot stat %s\n", path); // if fstat fails
        close(fd);
        return;
    }

    // if we are here we have successfully stored info in st

    if (st.type != T_DIR) { // st.type knows if its a dir, file or device
        close(fd);
        return;
    }
    // if  we are here, we have st and path is of dir
    // DIRSIZE is included in fs library, it is the 
    // maximum number of characters stored in a directory entry's 
    // name
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) { 
      printf("find: path too long\n");
      close(fd);
      return;
    }

    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    
        if (de.inum == 0)
        continue;
    
        memmove(p, de.name, DIRSIZ);
     
        p[DIRSIZ] = 0;
    
        if (strcmp(p, ".") == 0 || strcmp(p, "..") == 0 )
        continue;
    
    
        if (strcmp(p , search_name) == 0) {
            // execute task here
            if (isExec) {
                exec_command(buf, argv);
            } else { 
            printf("%s\n", buf);
            }
        }
    
        if (stat(buf, &st) < 0) {
        printf("find: cannot stat %s\n", buf);
        continue;
        }

    // If directory, search inside it
    if (st.type == T_DIR){
        find(buf, argv, isExec);
        }
    }
    close(fd);
}

int main (int argc, char *argv[]){
    int isExec = 0;
    if (argc >= 5 && strcmp(argv[3], "-exec") == 0) {
        isExec = 1;
        find (argv[1], argv, isExec);
    } else if (argc == 3) {
        find (argv[1], argv, isExec);
    }
    else {
        fprintf(2, "usage: find <directory> <target_name> [-exec cmd ...]\n");
        exit(1);
    }

    exit(0);
}