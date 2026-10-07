#include <kernel/types.h>
#include <kernel/stat.h>
#include <user/user.h>
#include <kernel/fcntl.h>

int is_delimiter(char c) {
    return c == ' ' || c == '-' || c == '\r' || 
           c == '\t' || c == '\n' || c == '.' ||
           c == '/' || c == ','; 
}

void print_number (int n) {
    char buf[16];
    int i = 0;

    if (n == 0) {
        write(1, "0\n", 2);
        return;
    }

    while (n > 0) {
        buf[i++] = '0' + (n % 10);
        n/=10;
    }
    while (i > 0) { write (1, &buf[--i], 1);}
    write(1, "\n", 1);
}

void process_file(char *filename){
    int fd;
    char c;
    char prev = 0;

    long number = 0;
    int in_number = 0;
    int valid_start = 0;

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        printf("sixfive: cannot open %s\n", filename);
        return;
    }

    while (read(fd, &c, 1) > 0) {
        
        if (c >= '0' && c <= '9') {
            if (!in_number){
                valid_start = (prev == 0 || is_delimiter(prev));
                number = 0;
                in_number = 1;
            }
            number = number * 10 + (c - '0');
        } else {
            if (in_number) {
                if (valid_start && is_delimiter(c)){
                    if (number % 5 == 0 || number % 6 == 0)
                        print_number(number);
                }
                in_number = 0;
                number = 0; 
            }
        }
        prev = c;
        
    }

    if (in_number && valid_start) {
        if (number % 5 == 0 || number % 6 == 0)
            print_number(number);
    }
    close(fd);
}

int main (int argc, char *argv[]){
    if (argc < 2) {
        printf("usage: sixfive file ...\n");
        exit(1);
    }
    
    for (int i = 1; i < argc; i++){
        process_file(argv[i]);
    }
    return 0;
}