#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "ptree.h"

void freeNode(struct TreeNode *root){
 
    if (root == NULL){
        return;
    }
    if (root->child_procs == NULL && root->next_sibling == NULL){
        free(root->name);
        free(root);
    }
    else{
        if(root->child_procs != NULL){
            freeNode(root->child_procs);
        }
        
        if(root->next_sibling != NULL){
            freeNode(root->next_sibling);
        }
        free(root->name);
        free(root);
    }
    
}


int main(int argc, char **argv) {
    // TODO: Update error checking and add support for the optional -d flag
    
    int depth = 0;
    
    if (argc < 2){
        fprintf(stderr, "Insufficient arguments provided\n");
        return 1;
    }
    
    if (argc > 4){
        fprintf(stderr, "Too many arguments provided\n");
        return 1;
    }
    
    if (argc == 3){
        fprintf(stderr, "Invalid number arguments provided\n");
        return 1;
    }
    
    if (argc == 4){
        
        if (argv[1][0] != '-' || argv[1][1] != 'd'){
            fprintf(stderr, "Invalid flag provided\n");
            return 1;
        }
        
        if (argv[2][0] != 48 && strtol(argv[2], NULL, 10) == 0){
            fprintf(stderr, "Depth argument must be an integer\n");
            return 1;
        }
        
        if (argv[3][0] != 48 && strtol(argv[3], NULL, 10) == 0){
            fprintf(stderr, "PID argument must be an integer\n");
            return 1;
        }
        
        else{
            depth = strtol(argv[2], NULL, 10);
        }
    }
    
    if (argc == 2){
        if (argv[1][0] != 48 && strtol(argv[1], NULL, 10) == 0){
            fprintf(stderr, "PID argument must be an integer\n");
            return 1;
        }
        
    }
    
    struct TreeNode *root = NULL;
    
    if (argc == 2){
        if (generate_ptree(&root, strtol(argv[1], NULL, 10)) == 1){
            fprintf(stderr, "Tree could not be created\n");
            return 2;
        }
        else{
            print_ptree(root, depth);
            freeNode(root);
        }
        
    }
    
    if (argc == 4){
        if (generate_ptree(&root, strtol(argv[3], NULL, 10)) == 1){
            fprintf(stderr, "Tree could not be created\n");
            return 2;
        }
        else{
            print_ptree(root, depth);
            freeNode(root);
        }
    
    }
    
    

    return 0;
}

