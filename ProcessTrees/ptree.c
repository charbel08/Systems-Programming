#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include "ptree.h"


// Defining the constants described in ptree.h
const unsigned int MAX_PATH_LENGTH = 1024;

// If TEST is defined (see the Makefile), will look in the tests 
// directory for PIDs, instead of /proc.

#ifdef TEST
    const char *PROC_ROOT = "tests";
#else
    const char *PROC_ROOT = "/proc";
#endif


/*
 * Creates a PTree rooted at the process pid. The root of the tree is
 * placed in root. The function returns 0 if the tree was created 
 * successfully and 1 if the tree could not be created or if at least
 * one PID was encountered that could not be found or was not an 
 * executing process.
 */
int generate_ptree(struct TreeNode **root, pid_t pid) {
    // Here's a trick to generate the name of a file to open. Note 
    // that it uses the PROC_ROOT variable
    
    char procfile_exe[MAX_PATH_LENGTH + 1];
    
    if (snprintf(procfile_exe, MAX_PATH_LENGTH + 1, "%s/%d/exe", PROC_ROOT, pid) < 0) {
        fprintf(stderr, "snprintf failed to produce an exe filename\n");
        return 1;
    }
    
    struct stat buffer2;
    
    if (lstat(procfile_exe, &buffer2) == -1){
        fprintf(stderr, "[PID]/exe file does not exist\n");
        return 1;
    }
    
    char procfile_cmd[MAX_PATH_LENGTH + 1];
    char procfile_children[MAX_PATH_LENGTH + 1];

    
    if (snprintf(procfile_cmd, MAX_PATH_LENGTH + 1, "%s/%d/cmdline", PROC_ROOT, pid) < 0) {
        fprintf(stderr, "snprintf failed to produce a cmdline filename\n");
        return 1;
    }
    
    if (snprintf(procfile_children, MAX_PATH_LENGTH + 1, "%s/%d/task/%d/children", PROC_ROOT, pid, pid) < 0) {
        fprintf(stderr, "snprintf failed to produce a children filename\n");
        return 1;
    }
    
    struct stat buffer3;
    
    if (lstat(procfile_cmd, &buffer3) == -1){
        fprintf(stderr, "[PID]/cmdline file does not exist\n");
        return 1;
    }
    
    struct TreeNode *true_node;
    true_node = malloc(sizeof(struct TreeNode));
    *root = true_node;
    true_node->next_sibling = NULL;
    true_node->child_procs = NULL;
    true_node->name = NULL;
    
    true_node->pid = pid;
    
    FILE *cmdline_file = fopen(procfile_cmd, "r");
    if (cmdline_file == NULL){
        fprintf(stderr, "Error opening file\n");
        return 1;
    }
    
    char cmd_path[MAX_PATH_LENGTH+1];
    cmd_path[0] = '\0';
    const char slash = '/';
    const char *colon = ":";
    char *name;
    
    
    fgets(cmd_path, MAX_PATH_LENGTH + 1, cmdline_file);
    
    if (cmd_path[0] == slash){
        name = strrchr(cmd_path, slash);
        name++;
    }
    
    else{
        name = strtok(cmd_path, colon);
    }
    
    
    if (name != NULL){
        true_node->name = malloc(sizeof(char)*(MAX_PATH_LENGTH+1));
        strncpy(true_node->name, name, MAX_PATH_LENGTH+1);
    }
    
    else{
        true_node->name = NULL;
    }

    
    if (fclose(cmdline_file) != 0){
        fprintf(stderr, "fclose failed\n");
        return 1;
    }
    
 
    struct stat buffer4;
    if (lstat(procfile_children, &buffer4) == -1){
        fprintf(stderr, "[PID]/children file does not exist\n");
        return 1;
    }
    
    
    FILE *children_file = fopen(procfile_children, "r");
    if (children_file == NULL){
        fprintf(stderr, "Error opening file\n");
        return 1;
    }
    
    
    char read_PID[MAX_PATH_LENGTH+1];
    read_PID[0] = '\0';
    
    struct TreeNode *tracker = NULL;
    
    if (fscanf(children_file, "%s", read_PID) == 1){
        int final_pid = strtol(read_PID, NULL, 10);
        generate_ptree(&(true_node->child_procs),final_pid);
        tracker = true_node->child_procs;
        
    }
    
    while(fscanf(children_file, "%s", read_PID) == 1){
        int final_pid = strtol(read_PID, NULL, 10);
        generate_ptree(&(tracker->next_sibling),final_pid);
        tracker = tracker->next_sibling;
        tracker->next_sibling = NULL;
    }
    
    if (fclose(children_file) != 0){
        fprintf(stderr, "fclose failed\n");
        return 1;
    }
    
    
    return 0;
}

/*
 * Prints the TreeNodes encountered on a preorder traversal of an PTree
 * to a specified maximum depth. If the maximum depth is 0, then the 
 * entire tree is printed.
 */
void print_ptree(struct TreeNode *root, int max_depth) {
    // Here's a trick for remembering what depth (in the tree) you're at
    // and printing 2 * that many spaces at the beginning of the line.
    static int depth = 0;
    
    // Your implementation goes here.
    
    if (root == NULL){
        return;
    }
    if (root->name == NULL){
        printf("%*s%d\n", depth * 2, "", root->pid);
    }
  
    else{
        printf("%*s%d: %s\n", depth * 2, "", root->pid, root->name);
    }
        
    if (depth < max_depth - 1 || max_depth == 0){
        if (root->child_procs != NULL){
            depth++;
            print_ptree(root->child_procs, max_depth);
            depth--;
                
            }
        }
    if (root->next_sibling != NULL){
        print_ptree(root->next_sibling, max_depth);
            
    }
    
}

