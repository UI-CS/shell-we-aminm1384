#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
  setbuf(stdout, NULL);
  printf("$ ");
  
  int max_size = 1024;
  char input[max_size];

  while (fgets(input, max_size, stdin) !=NULL) {

  input[strlen(input) - 1] = '\0';
  if (strncmp(input, "exit", 4) == 0) {
      char *rest = input + 4; 

      
      while (*rest == ' ') rest++;

      if (*rest == '\0') {
          
          return 0;
      } else {
          
          int status = atoi(rest); 
          return status;
      }
  }



  else if (strncmp(input, "echo", 4) == 0) {
        if (strlen(input) == 4) {
            printf("\n");
        } else if (input[4] == ' ') {
            printf("%s\n", input + 5);
        } else {
            printf("%s: command not found\n", input);
        }
    }


  
  else if (strncmp(input, "type ", 5) == 0) {
    char *cmd_name = input + 5; 

       
    while (*cmd_name == ' ') cmd_name++;

      
    if (strcmp(cmd_name, "exit") == 0 ||
        strcmp(cmd_name, "echo") == 0 ||
        strcmp(cmd_name, "type") == 0) {
        printf("%s is a shell builtin\n", cmd_name);
        } else {
            char *path = getenv("PATH");
            if (path == NULL){
                printf("path does not exist\n");
                return 0;
            }
            else{
                int found = 0;
                char path_copy[4096];
                strcpy(path_copy, path);
                char *token = strtok(path_copy, ":");
                
                while (token !=NULL) {
                    char full_path[1024];
                    snprintf(full_path, sizeof(full_path), "%s/%s",token ,cmd_name );

                    if (access(full_path, X_OK)== 0){
                        printf("%s is %s\n",cmd_name, full_path);
                        found = 1;
                        break;
                    }
                    token = strtok(NULL, ":");
                }
                if (found == 0){
                    printf("%s: not found\n",cmd_name);
                }
            }
        }
    }
    

    else {
        printf("%s: command not found\n", input);
    }



  printf("$ ");

  }


  return 0;
}
