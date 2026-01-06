#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>

int main() {


    setbuf(stdout, NULL);

    int should_run = 1;
    int max_size = 1024;

    char *args[64];
    char input[max_size];
    char last_command[1024] = {0}; 

    while (should_run) {

        while (waitpid(-1, NULL, WNOHANG) > 0);
    
        printf("uinxsh> ");

        if (fgets(input, max_size, stdin)== NULL){
            break;
        }

        input[strlen(input) - 1] = '\0';
        
        //---parsing
        int i = 0;
        char *token = strtok(input, " ");
        while (token != NULL && i < 63) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;


        if (strcmp(args[0], "!!") == 0) {

            if (strlen(last_command) == 0) {
                printf("No commands in history\n");
                continue;
            }

            printf("%s\n", last_command);
            strcpy(input, last_command);


            i = 0;
            token = strtok(input, " ");
            while (token != NULL && i < 63) {
                args[i++] = token;
                token = strtok(NULL, " ");
            }
            args[i] = NULL;
        }

        
        if (args[0] == NULL) {
                    continue;
                }

        if (strcmp(args[0], "!!") != 0) {
            strcpy(last_command, input);
        }


        int background = 0;

        if (i > 0 && strcmp(args[i - 1], "&") == 0) {
            background = 1;
            args[i - 1] = NULL; // حذف &
        }

 
        //---exit 
        if (strcmp(args[0], "exit") == 0) {

            int status = 0;

            if (args[1] != NULL) {
                status = atoi(args[1]);
            }

            should_run = 0;
            return status;
        }



        //---echo 
        else if (strcmp(args[0], "echo") == 0) {
            for (int j = 1; args[j] != NULL; j++) {
                printf("%s", args[j]);
                if (args[j + 1] != NULL)
                    printf(" ");
            }
            printf("\n");
        }


        //---pwd
        else if (strcmp(args[0], "pwd") == 0) {
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                printf("%s\n", cwd);
            } else {
                perror("pwd");
            }
        }


        //---cd
        else if (strcmp(args[0], "cd") == 0) {
            char *path = args[1];

            if (path == NULL) {
                path = getenv("HOME");
            }

            if (chdir(path) != 0) {
                perror("cd");
            }
        }
  
        //---type
        else if (strcmp(args[0], "type") == 0) {

            if (args[1] == NULL) {
                printf("type: missing argument\n");
            }
            else if (
                strcmp(args[1], "exit") == 0 ||
                strcmp(args[1], "echo") == 0 ||
                strcmp(args[1], "type") == 0 ||
                strcmp(args[1], "cd")   == 0 ||
                strcmp(args[1], "pwd")  == 0
            ) {
                printf("%s is a shell builtin\n", args[1]);
            }
            else {
                char *path = getenv("PATH");
                int found = 0;

                if (path != NULL) {
                    char path_copy[4096];
                    strcpy(path_copy, path);

                    char *dir = strtok(path_copy, ":");
                    while (dir != NULL) {
                        char full_path[1024];
                        snprintf(full_path, sizeof(full_path),"%s/%s", dir, args[1]);

                        if (access(full_path, X_OK) == 0) {
                            printf("%s is %s\n", args[1], full_path);
                            found = 1;
                            break;
                        }
                        dir = strtok(NULL, ":");
                    }
                }

                if (!found) {
                    printf("%s: not found\n", args[1]);
                }
            }
        }

        else {
            pid_t pid = fork();

            if (pid < 0) {
                perror("fork failed");
            }

            //---child
            else if (pid == 0) {
                execvp(args[0], args);
                perror("command not found");
                exit(1);
            }

            //---parent
            else {
                if (!background) {
                    wait(NULL);   // foreground
                } else {
                    printf("[running in background] pid=%d\n", pid);
                }
            }
        }




  }

   return 0;
}





