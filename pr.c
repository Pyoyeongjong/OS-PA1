#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#define MAX 255

int main(void){
        char *a = "ls";
        char *b = "-al";
        char *c = "ls";
        char *d=  "ls";

        char * const argv[] = {a, b};
        //execlp("ls", "ls", NULL);
        execvp("ls", argv);
        //execlp("ls", "ls", "-al", NULL);
        //execlp("cp", "cp", "pa1.c", "pa11.c", NULL);
        chdir(getenv("Home"));
        
        return 0;
}