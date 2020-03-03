#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <math.h>

#include "eratosthenes.h"

int main(int argc, char *argv[]) {
    // Turning off sigpipe
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("signal");
        exit(1);
    }
    
    if (argc > 2 || argc == 1){
        fprintf(stderr, "Usage:\n\tpfact n\n");
        exit(1);
    }
    
    char *strcheck;
    int n = strtol(argv[1], &strcheck, 10);
    if (strlen(strcheck) != 0) {
        fprintf(stderr, "Usage:\n\tpfact n\n");
        exit(1);
    }
    if (n <= 0 || n == 1){
        fprintf(stderr, "Usage:\n\tpfact n\n");
        exit(1);
    }
    
    int fd[2];
    int *fds = malloc(2*sizeof(int));

    if (pipe(fd) == -1) {
        exit(2);
    }
    
    int factor1 = 0;
    int factor2 = 0;
    
    int status;
    pid_t pid = fork();
    
    // ERROR
    if (pid < 0){
        exit(2);
    }
    
    // PARENT
    if (pid > 0){
        
        if (close(fd[0]) < 0){
            exit(2);
        }
        
        for(int i = 2; i <= sqrt(n); i++){
            write(fd[1], &i, sizeof(int));
        }
        
        if (close(fd[1]) < 0){
            exit(2);
        }
        
        while (wait(&status) > 0);
        free(fds);
        
        if (WEXITSTATUS(status) < 255) {
            printf("Number of filters = %d\n", WEXITSTATUS(status));
            return 0;
        }
        
        else{
            return 2;
        }
    }
    
    // CHILD
    else {
        if (close(fd[1]) < 0){
            exit(2);
        }
        
        int m;
        while( (read(fd[0], &m, sizeof(int)) != 0) ){
            if (n % (m * m) == 0 && m*m == n){
                    factor1 = m;
                    factor2 = m;
                    break;
                }
            
            if (n % m == 0){
                if ((n / m) % m == 0){
                    printf("%d is not the product of two primes\n", n);
                    exit(0);
                }
            }
            
            if (n % m == 0 && m != n){
                if (factor1 == 0){
                    factor1 = m;
                    
                }
                else if (factor2 == 0){
                    factor2 = m;
                    printf("%d is not the product of two primes\n", n);
                    exit(0);
                }
                else{
                    printf("%d is not the product of two primes\n", n);
                    exit(0);
                }
            }
            
            if (factor1 != 0 && factor2 != 0 && m < sqrt(n)){
                printf("%d is not the product of two primes\n", n);
                exit(0);
                
            }
            
            int make_pid = make_stage(m, fd[0], &fds);
            if (make_pid > 0){
               if (WEXITSTATUS(status) != 255){
                   while (waitpid(make_pid, &status, 0) > 0);
                   exit(WEXITSTATUS(status) + 1);
                }
               else {
                   exit(255);
               }
            }
            dup2(fds[0], fd[0]);

        }
        

        if (factor1 == 0 && factor2 == 0){
            printf("%d is prime\n", n);
            exit(0);
        }
        

        if (factor1 != 0 && factor2 == 0){
            factor2 = n / factor1;
        }
       
        
        if (factor1 != 0 && factor2 != 0){
            printf("%d %d %d\n", n, factor1, factor2);
            exit(0);
        }
        
    }
    
    return 0;
}
