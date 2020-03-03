#include <stdio.h>

#include "eratosthenes.h"

int filter(int n, int readfd, int writefd) {
    // TODO: Complete
    
    int read_number;
    int read_return_value;
    while ( (read_return_value = read(readfd, &read_number, sizeof(int)) != 0) ){
        if (read_return_value == -1){
            return 1;
        }
        if (read_number % n != 0){
            if (write(writefd, &read_number, sizeof(int)) == -1){
                return 1;
            }
        }
    }
    
    return 0;
}

