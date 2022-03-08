#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void getCommandArguments(char* commandArgs[], char command[]){
  int i = 0;
  char *token = strtok(command, " \n \r \0");

  while (token != NULL)
  {
    commandArgs[i] = token;
    token = strtok(NULL, " \n \r \0");
    i++;    
  }
}

void doCommand(char command[], char *path) {
  char error_message[30] = "An error has occurred\n";
  char fork_error_msg[30] = "Fork failed\n";
  char *commandArgs[10] = { NULL };
  getCommandArguments(commandArgs, command);

  if(strcmp(commandArgs[0], "exit") == 0) {
    if (commandArgs[1] == NULL) {
      exit(0);
    } else {
      write(STDERR_FILENO, error_message, strlen(error_message));
    }
  } else if(strcmp(commandArgs[0], "cd") == 0) {
    if(commandArgs[1] != NULL && commandArgs[2] == NULL) {
      char *dir = commandArgs[1];
      if (chdir(dir) == 0) {
        
      } else {
        write(STDERR_FILENO, error_message, strlen(error_message));
      } 
    } else {
      write(STDERR_FILENO, error_message, strlen(error_message));
    }
  } else if(strcmp(commandArgs[0], "ls") == 0) {
    int rc = fork();
    if(rc < 0) {
      write(STDERR_FILENO, fork_error_msg, strlen(fork_error_msg));
      exit(1);
    } else if(rc == 0) {
      execv("/bin/ls", commandArgs);
    } else {
      wait(&rc);
    }
  } else if(strcmp(commandArgs[0], "path") == 0) {
    int argsIndex = 1;
    int pathIndex = 0;
    if (commandArgs[argsIndex] != NULL) {
     
      while (commandArgs[argsIndex] != NULL) {
        printf("Copy commannd[%d]: %s\n Into path[%d]: %s\n", argsIndex, commandArgs[argsIndex], pathIndex, &path[pathIndex]);
        strcpy(path+pathIndex, commandArgs[argsIndex]);
        //strcpy(&path[pathIndex], commandArgs[argsIndex]);
        argsIndex++;
        pathIndex++;
        printf("Copied!\n");
      }
    } else {
      for(int i = 0; i < 20; i++) {
        strcpy(&path[i], "");
      }
    }

    for(int i = 0; i < 20; i++) {
      printf("Path[%d]: %s\n", i, &path[i]);
    }

    printf("Path[0]: %s\n", &path[0]);
    printf("Path[1]: %s\n", &path[1]);
      
  
  } else {
    if (strcmp(path+0, "")) {
      const char *filename = "./tests/p1.sh";

       int fd = access(filename, X_OK);
    if(fd == 0){
      printf("Can get file\n");
    }
    else{
       printf("Error can't get file\n");
    }
      write(STDERR_FILENO, error_message, strlen(error_message));
    } else {
      printf("Path in commands:\n");
      for(int i = 0; i < 20; i++) {
        printf("Path: %s\n", path+i);
        if (strcmp(path+i, "")) {
          break;
        }
      }
      printf("-------------\n");
      /*
      for(int i = 0; i < 20; i++) {
        printf("Path: %s\n", path[i]);
      }

      for(int i = 0; i < 20; i++) {
        printf("CmdArgs: %s\n", commandArgs[i]);
      }
      */
      
      if (commandArgs[1] == NULL) {
        printf("File: %s\n", commandArgs[0]);
        int foundFile = 0;
        for(int i = 0; i < 20; i++) {
          if(foundFile == 0 && !strcmp(path+i, "")){
            printf("Checking path: %s\n", path+i);
            int pathLen = strlen(path+i);
            int cmdLen = strlen(commandArgs[0]);
            char *dir = malloc( pathLen + cmdLen + 1 );
            int result;
            
            strcpy(dir, "./");
            strcat(dir, path+i);
            strcat(dir, "/");
            strcat(dir, commandArgs[0]);

            printf("Checking dir: %s\n", dir);

            result = access(dir, X_OK);
            if (result == 0) {
              printf("Found exe: %s\n", dir);
              foundFile = 1;
              int rc = fork();
              
              if(rc < 0) {
                write(STDERR_FILENO, fork_error_msg, strlen(fork_error_msg));
                exit(1);
              } else if(rc == 0) {
                execv(dir, commandArgs);
              } else {
                wait(&rc);
              }
            }else{
              printf("No exe found.\n");
            }

            free(dir);
            for(int i = 0; i < 20; i++) {
              printf("Path: %s\n", path+i);
            }
          } else {
            break;
          }
        }  
      } else {
        printf("Multiargs\n");
      }
    }
  }
}

void interactive()
{
  char input[255];
  char path[20][255] = { "" };
  char error_message[30] = "An error has occurred\n";
  char fork_error_msg[30] = "Fork failed\n";
  
  for(int i = 0; i < 20; i++) {
    strcpy(path[i], "");
  }
  strcpy(path[0], "bin");

  while(1) 
  {
    char *commandArgs[10] = { NULL };
    
    printf("wish> ");
    scanf("%255[^\n]%*c", input);

    getCommandArguments(commandArgs, input);

    if(strcmp(commandArgs[0], "exit") == 0) {
      if (commandArgs[1] == NULL) {
        exit(0);
      } else {
        write(STDERR_FILENO, error_message, strlen(error_message));
      }
    } else if(strcmp(commandArgs[0], "cd") == 0) {
      if(commandArgs[1] != NULL && commandArgs[2] == NULL) {
        char *dir = commandArgs[1];
        if (chdir(dir) == 0) {
          
        } else {
          write(STDERR_FILENO, error_message, strlen(error_message));
        } 
      } else {
        write(STDERR_FILENO, error_message, strlen(error_message));
      }
    } else if(strcmp(commandArgs[0], "path") == 0) {
      int argsIndex = 1;
      int pathIndex = 0;
      if (commandArgs[argsIndex] != NULL) {
        for(int i = 0; i < 20; i++) {
          strcpy(path[i], "");
        }
        while (commandArgs[argsIndex] != NULL) {
          strcpy(path[pathIndex], commandArgs[argsIndex]);
          argsIndex++;
          pathIndex++;
        }
      } else {
        for(int i = 0; i < 20; i++) {
          strcpy(path[i], "");
        }
      }
    } else {
      if (strcmp(path[0], "") == 0) {
        write(STDERR_FILENO, error_message, strlen(error_message));
      } else {
        if (commandArgs[0] != NULL) {
          int foundFile = 0;
          for(int i = 0; i < 20; i++) {
            if(foundFile == 0 && strcmp(path[i], "") != 0){
              
              int pathLen = strlen(path[i]);
              int cmdLen = strlen(commandArgs[0]);
              char *dir = malloc( pathLen + cmdLen + 1 );
              int result;

              if(strcmp(commandArgs[0], "ls") == 0){
                  strcpy(dir, "/");
                } else {
                  strcpy(dir, "./"); 
                }
                strcat(dir, path[i]);
                strcat(dir, "/");
                strcat(dir, commandArgs[0]);

                result = access(dir, X_OK);
                if (result == 0) {
                  foundFile = 1;
                  int rc = fork();
                  
                  if(rc < 0) {
                    write(STDERR_FILENO, fork_error_msg, strlen(fork_error_msg));
                    exit(1);
                  } else if(rc == 0) {
                    execv(dir, commandArgs);
                  } else {
                    wait(&rc);
                  }
                } else {
                  write(STDERR_FILENO, error_message, strlen(error_message));
                }
              free(dir);
            } else {
              break;
            }
          }  
        } else {
          //printf("Multiargs\n");
        }
      }
    }
  }
}

void batch(char *textFile) {
  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  char path[20][255] = { "bin" };
  char error_message[30] = "An error has occurred\n";
  char fork_error_msg[30] = "Fork failed\n";

  fp = fopen(textFile, "r");
  if (fp == NULL) {
    exit(0);
  }

  while ((read = getline(&line, &len, fp)) != -1) {
    char *commandArgs[10] = { NULL };
    getCommandArguments(commandArgs, line);

    if(strcmp(commandArgs[0], "exit") == 0) {
      if (commandArgs[1] == NULL) {
        exit(0);
      } else {
        write(STDERR_FILENO, error_message, strlen(error_message));
      }
    } else if(strcmp(commandArgs[0], "cd") == 0) {
      if(commandArgs[1] != NULL && commandArgs[2] == NULL) {
        char *dir = commandArgs[1];
        if (chdir(dir) == 0) {
          
        } else {
          write(STDERR_FILENO, error_message, strlen(error_message));
        } 
      } else {
        write(STDERR_FILENO, error_message, strlen(error_message));
      }
    } else if(strcmp(commandArgs[0], "path") == 0) {
      int argsIndex = 1;
      int pathIndex = 0;
      if (commandArgs[argsIndex] != NULL) {
        for(int i = 0; i < 20; i++) {
          strcpy(path[i], "");
        }
        
        while (commandArgs[argsIndex] != NULL) {
          strcpy(path[pathIndex], commandArgs[argsIndex]);
          argsIndex++;
          pathIndex++;
        }
      } else {
        for(int i = 0; i < 20; i++) {
          strcpy(path[i], "");
        }
      }
    } else {
      if (strcmp(path[0], "") == 0) {
        write(STDERR_FILENO, error_message, strlen(error_message));
      } else {
        if (commandArgs[0] != NULL) {
          int foundFile = 0;
          for(int i = 0; i < 20; i++) {
            if(foundFile == 0 && strcmp(path[i], "") != 0){
              
              int pathLen = strlen(path[i]);
              int cmdLen = strlen(commandArgs[0]);
              char *dir = malloc( pathLen + cmdLen + 1 );
              int result;

              if(strcmp(commandArgs[0], "ls") == 0){
                  strcpy(dir, "/");
              } else {
                  strcpy(dir, "./"); 
              }
                strcat(dir, path[i]);
                strcat(dir, "/");
                strcat(dir, commandArgs[0]);

                result = access(dir, X_OK);
                if (result == 0) {
                  foundFile = 1;
                  int rc = fork();
                  
                  if(rc < 0) {
                    write(STDERR_FILENO, fork_error_msg, strlen(fork_error_msg));
                    exit(1);
                  } else if(rc == 0) {
                    execv(dir, commandArgs);
                  } else {
                    wait(&rc);
                  }
                } else {
                  write(STDERR_FILENO, error_message, strlen(error_message));
                }
              free(dir);
            } else {
              break;
            }
          }  
        } else {
          //printf("Multiargs\n");
        }
      }
    }
  }

  fclose(fp);
  free(line);
}