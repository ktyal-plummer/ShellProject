#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
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

void createFork(char* dir, char* commandArgs[]){
  char fork_error_msg[30] = "Fork failed\n";
  int rc = fork();
                  
  if(rc < 0) {
    write(STDERR_FILENO, fork_error_msg, strlen(fork_error_msg));
    exit(1);
  } else if(rc == 0) {
    execv(dir, commandArgs);
  } else {
    wait(&rc);
  }
}

void interactive()
{
  char input [255];
  char path[20][255] = { "" };

  char error_message[30] = "An error has occurred\n";
  char fork_error_msg[30] = "Fork failed\n";

  // initialize path 
  for(int i = 0; i < 20; i++) {
    strcpy(path[i], "");
  }
  strcpy(path[0], "bin");

  while(1) 
  {
    int correctInput = 1;
    int outputFile = 0;
    
    // reset command arguments and get new arguments
    char *commandArgs[10] = { NULL };
    printf("wish> ");
    scanf("%255[^\n]%*c", input);
    getCommandArguments(commandArgs, input);

    /*test print
    for(int i = 0; i < 3; i++){
      printf("Command[%d]: %s\n", i, commandArgs[i]);
    }*/

    // parse input for output file
    int foundOutput = 0;
    int parseIndex = 0;
    int fileIndex = 0;
    char argument[255] = "";
    char *fileArgs[3] = { NULL };
    
    while (commandArgs[parseIndex] != NULL) {
      int hasOutput = 0;
      if(strchr(commandArgs[parseIndex], '>') != NULL)
      {
        hasOutput = 1;
        foundOutput = 1;
      }

      if (hasOutput) {
        strcpy(argument, commandArgs[parseIndex]);
        printf("Arg: %s\n", argument);
        char *token = strtok(argument, ">");
        
        while( token != NULL ) {
          fileArgs[fileIndex] = token;
          token = strtok(NULL, ">");
          fileIndex++;
        }
      }
      parseIndex++;
    }

    //test print
    for(int i = 0; i < 3; i++){
      printf("FilesArg[%d]: %s\n", i, fileArgs[i]);
    }

    // error if incorrect file input
    if (foundOutput){
      if(fileArgs[0] == NULL){
        correctInput = 0;
        write(STDERR_FILENO, error_message, strlen(error_message));
      } else if(fileArgs[1] != NULL){
        if(fileArgs[2] == NULL){
          outputFile = 1;
          //printf("Valid input and output file\n");
        } else {
          correctInput = 0;
          write(STDERR_FILENO, error_message, strlen(error_message));
        }
      }
    }

    // commands
    if(correctInput){
      // exit 
      if(strcmp(commandArgs[0], "exit") == 0) {
        if (commandArgs[1] == NULL) {
          exit(0);
        } else {
          write(STDERR_FILENO, error_message, strlen(error_message));
        }
      // cd
      } else if(strcmp(commandArgs[0], "cd") == 0) {
        if(commandArgs[1] != NULL && commandArgs[2] == NULL) {
          char *dir = commandArgs[1];
          if (chdir(dir) != 0) {
            write(STDERR_FILENO, error_message, strlen(error_message));
          } 
        } else {
          write(STDERR_FILENO, error_message, strlen(error_message));
        }
      // path
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
      // other
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
                int result = 1;
  
                strcat(dir, path[i]);
                strcat(dir, "/");
                strcat(dir, commandArgs[0]);
  
                if (access(dir, X_OK) == 0) {
                  result = 0;
                } else {
                  char *startSlashes[2] = {"/", "./"};
                  for(int i = 0; i < 1; i++) {
                    strcpy(dir, "");
                    strcpy(dir, startSlashes[i]);
                    strcat(dir, path[i]);
                    strcat(dir, "/");
                    strcat(dir, commandArgs[0]);
    
                    result = access(dir, X_OK);
                    if (!result) { break; }
                  }
                }
                
                if (result == 0) {
                  foundFile = 1;
                  
                  int rc = fork();   
                  if(rc < 0) {
                    write(STDERR_FILENO, fork_error_msg, strlen(fork_error_msg));
                    exit(1);
                  } else if(rc == 0) {
                    if(outputFile){
                      int out = open(fileArgs[1], O_RDWR|O_CREAT|O_APPEND, 0600);
                      if (out == -1) { 
                        write(STDERR_FILENO, error_message, strlen(error_message)); 
                      }
                      int save_out = dup(fileno(stdout));
                      if (-1 == dup2(out, fileno(stdout))) { 
                        write(STDERR_FILENO, error_message, strlen(error_message));
                      }
                      
                      strcpy(commandArgs[1], fileArgs[0]);
                      execv(dir, commandArgs);

                      fflush(stdout); 
                      close(out);
                      dup2(save_out, fileno(stdout));
                      close(save_out);
                    } else {
                       execv(dir, commandArgs);
                    }
                  } else {
                    wait(&rc);
                  }
                }

                
                
                free(dir);
              } else {
                break;
              }
            }  
            if (!foundFile) {
              write(STDERR_FILENO, error_message, strlen(error_message));
            }
          } else {
            //printf("Multiargs\n");
          }
        }
      }
    }
  }
}

void batch(int numArgs, char *textFile) {
  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  char path[20][255] = { "" };
  strcpy(path[0], "bin");
  char error_message[30] = "An error has occurred\n";
  char fork_error_msg[30] = "Fork failed\n";

  if(numArgs > 2) {
    write(STDERR_FILENO, error_message, strlen(error_message));
    exit(1);
  }

  fp = fopen(textFile, "r");
  if (fp == NULL) {
    exit(0);
  }

  while ((read = getline(&line, &len, fp)) != -1) {
    char *commandArgs[10] = { NULL };
    getCommandArguments(commandArgs, line);

    // parse input for output file
    int correctInput = 1;
    int outputFile = 0;
    int foundOutput = 0;
    int parseIndex = 0;
    int fileIndex = 0;
    char argument[255] = "";
    char *fileArgs[3] = { NULL };
    
    while (commandArgs[parseIndex] != NULL) {
      int hasOutput = 0;
      if(strchr(commandArgs[parseIndex], '>') != NULL)
      {
        hasOutput = 1;
        foundOutput = 1;
      }

      if (hasOutput) {
        strcpy(argument, commandArgs[parseIndex]);
        char *token = strtok(argument, ">");
        
        while( token != NULL ) {
          fileArgs[fileIndex] = token;
          token = strtok(NULL, ">");
          fileIndex++;
        }
      }
      parseIndex++;
    }

    // error if incorrect file input
    if (foundOutput){
      if(fileArgs[0] == NULL){
        correctInput = 0;
        write(STDERR_FILENO, error_message, strlen(error_message));
      } else if(fileArgs[1] != NULL){
        if(fileArgs[2] == NULL){
          outputFile = 1;
          //printf("Valid input and output file\n");
        } else {
          correctInput = 0;
          write(STDERR_FILENO, error_message, strlen(error_message));
        }
      }
    }

    // commands
    if(correctInput){
      // exit 
      if(strcmp(commandArgs[0], "exit") == 0) {
        if (commandArgs[1] == NULL) {
          exit(0);
        } else {
          write(STDERR_FILENO, error_message, strlen(error_message));
        }
      // cd
      } else if(strcmp(commandArgs[0], "cd") == 0) {
        if(commandArgs[1] != NULL && commandArgs[2] == NULL) {
          char *dir = commandArgs[1];
          if (chdir(dir) != 0) {
            write(STDERR_FILENO, error_message, strlen(error_message));
          } 
        } else {
          write(STDERR_FILENO, error_message, strlen(error_message));
        }
      // path
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
      // other
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
                int result = 1;
  
                strcat(dir, path[i]);
                strcat(dir, "/");
                strcat(dir, commandArgs[0]);
  
                if (access(dir, X_OK) == 0) {
                  result = 0;
                } else {
                  char *startSlashes[2] = {"/", "./"};
                  for(int i = 0; i < 1; i++) {
                    strcpy(dir, "");
                    strcpy(dir, startSlashes[i]);
                    strcat(dir, path[i]);
                    strcat(dir, "/");
                    strcat(dir, commandArgs[0]);
    
                    result = access(dir, X_OK);
                    if (!result) { break; }
                  }
                }
                
                if (result == 0) {
                  foundFile = 1;
                  
                  int rc = fork();   
                  if(rc < 0) {
                    write(STDERR_FILENO, fork_error_msg, strlen(fork_error_msg));
                    exit(1);
                  } else if(rc == 0) {
                    if(outputFile){
                      int out = open(fileArgs[1], O_RDWR|O_CREAT|O_APPEND, 0600);
                      if (out == -1) { 
                        write(STDERR_FILENO, error_message, strlen(error_message)); 
                      }
                      int save_out = dup(fileno(stdout));
                      if (-1 == dup2(out, fileno(stdout))) { 
                        write(STDERR_FILENO, error_message, strlen(error_message));
                      }
                      
                      strcpy(commandArgs[1], fileArgs[0]);
                      execv(dir, commandArgs);

                      fflush(stdout); 
                      close(out);
                      dup2(save_out, fileno(stdout));
                      close(save_out);
                    } else {
                       execv(dir, commandArgs);
                    }
                  } else {
                    wait(&rc);
                  }
                }    
                
                free(dir);
              } else {
                break;
              }
            }  
            if (!foundFile) {
              write(STDERR_FILENO, error_message, strlen(error_message));
            }
          } else {
            //printf("Multiargs\n");
          }
        }
      }
    }
  }

  fclose(fp);
  free(line);
}