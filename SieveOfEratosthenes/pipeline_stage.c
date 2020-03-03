#include <unistd.h>
#include <stdlib.h>

#include "eratosthenes.h"

pid_t make_stage(int m, int read_fd, int **fds) {
    int *actual_fds = *fds;
    
    if (pipe(actual_fds) == -1) {
        exit(255);
    }
    
    pid_t pid = fork();
    
    // ERROR
    if (pid < 0){
        exit(255);
    }
    
    // PARENT
    else if (pid > 0){
        if (close(actual_fds[0]) == -1){
            exit(255);
        }
        
        
        filter(m, read_fd, actual_fds[1]);
        
        if (close(read_fd) == -1){
            exit(255);
        }
        
        if (close(actual_fds[1]) == -1){
            exit(255);
        }
    
        
        return pid;
        
    }
    
    // CHILD
    else{
        if (close(actual_fds[1]) == -1){
            exit(255);
        }

        return 0;
        
    }
    
}
