#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

struct Map {
    char key[64];
    char value[64];
} storedVariables[50];

int lastExitCode = 0;
int variableIndex = 0;

// define functions outside of main()
int analyzeCommand(char command[64]);
int runProgram(char command[64]);
int executeOutputRedirection(char command[64]);
int executeInputRedirection(char command[64]);
int defineVariable(char command[64]);
char *replaceAllVariables(char command[64]);
char *findValueInArrayByKey(char *key);

int main(int argc, char **argv) {
  while(true) {
    fflush(stdout);
    printf("$ ");
    fflush(stdout);
    char command[64];
    int scanfResult = scanf("%[^\n]", command);
    if (scanfResult != EOF && scanfResult != 0) {
      // scanf leaves one \n in the buffer, getChar takes it
      getchar();
      fflush(stdin);
      fflush(stdout);
      lastExitCode = analyzeCommand(command);
    } else break;
  }
  return 0;
}

int analyzeCommand(char command[64]) {
  if(strstr(command, "$") != NULL) {
    command = replaceAllVariables(command);
    if (command == NULL) return EXIT_FAILURE;
  }
  
  if(strstr(command, "=") != NULL) return defineVariable(command);
  else if(strstr(command, "true") != NULL) return EXIT_SUCCESS;
  else if(strstr(command, "false") != NULL) return EXIT_FAILURE;
  else if(strcmp(command, "echo &?") == 0) {
    printf("%d\n", lastExitCode);
    return EXIT_SUCCESS;
  }
  else if(strstr(command, ">") != NULL) return executeOutputRedirection(command);
  else if(strstr(command, "<") != NULL) return executeInputRedirection(command);
  return runProgram(command);
}

int runProgram(char command[64]) {  
  pid_t pid = fork();
  if (pid == -1) return EXIT_FAILURE;
  if (pid > 0) {
    // we're the parent
    if(strstr(command, "&") == NULL) {
      // we have to wait!
      int status;
      waitpid(pid, &status, 0);
    }
    return EXIT_SUCCESS;

  } else {
    char *token = strtok(command, " ");
    char **args = malloc(64*sizeof(char*));
    int index = 0;
    while (token != NULL) {
      args[index] = token;
      index++;    
      token = strtok(NULL, " ");
    }
    // inserting the null pointer to finish the array
    args[index] = NULL;
    int returnCode = execvp(args[0], args);
    free(args);
    return returnCode;
  }
}

int executeOutputRedirection(char command[64]) {
  pid_t pid = fork();
  int returnCode = EXIT_SUCCESS;
  if (pid == -1) return EXIT_FAILURE;
  if (pid > 0) {
    int status;
    waitpid(pid, &status, 0);
    return EXIT_SUCCESS;
  }
  if (pid == 0) {
    // we're the child
    char *beforeRedirect = strtok(command, ">");
    char *fileName = strtok(NULL, ">");
    strncpy(fileName, fileName+1, strlen(fileName));

    char *commandBeforeRedirect = strtok(beforeRedirect, " ");
    char **args = malloc(64*sizeof(char*));
    int index = 0;
    while (commandBeforeRedirect != NULL) {
      args[index] = commandBeforeRedirect;
      index++;
      commandBeforeRedirect = strtok(NULL, " ");
    }
    // inserting the null pointer to finish the array
    args[index] = NULL;

    FILE *output;
    if((output = fopen(fileName, "w")) == NULL) return EXIT_FAILURE;
    int fd = fileno(output);
    dup2(fd, STDOUT_FILENO);
    fclose(output);

    returnCode = execvp(args[0], args);
    free(args);
  }
  return returnCode;
}

int executeInputRedirection(char command[64]) {
  pid_t pid = fork();
  int returnCode = EXIT_SUCCESS;
  if (pid == -1) return EXIT_FAILURE;
  if (pid > 0) {
    int status;
    waitpid(pid, &status, 0);
    return EXIT_SUCCESS;
  }
  if (pid == 0) {
    // we're the child
    char *beforeRedirect = strtok(command, "<");
    char *fileName = strtok(NULL, "<");
    // strncpy(fileName, fileName+1, strlen(fileName));

    char *commandBeforeRedirect = strtok(beforeRedirect, " ");
    char **args = malloc(64*sizeof(char*));
    int index = 0;
    while (commandBeforeRedirect != NULL) {
      args[index] = commandBeforeRedirect;
      index++;
      commandBeforeRedirect = strtok(NULL, " ");
    }
    // inserting the null pointer to finish the array
    args[index] = NULL;

    FILE *input;
    if((input = fopen(fileName, "r")) == NULL) return EXIT_FAILURE;
    int fd = fileno(input);
    dup2(fd, STDIN_FILENO);
    fclose(input);

    returnCode = execvp(args[0], args);
    free(args);
  }
  return returnCode;
}

int defineVariable(char command[64]) {
  char *variableName = strtok(command, "=");
  char *variableValue = strtok(NULL, "=");
  strcpy(storedVariables[variableIndex].key, variableName);
  strcpy(storedVariables[variableIndex].value, variableValue);
  variableIndex++;
  return EXIT_SUCCESS;
}

char *replaceAllVariables(char command[64]) {
  // first, scan for ${}
  while(strstr(command, "${") != NULL) {
    int numOfCharsBeforeVariable = strcspn(command, "{");
    char stringStartingWithVariable[64] = "";
    strncpy(stringStartingWithVariable, command + numOfCharsBeforeVariable +1, strlen(command) - numOfCharsBeforeVariable -1);
    
    int variableLength = strcspn(stringStartingWithVariable, "}");
    char variable[64] = "";
    strncpy(variable, stringStartingWithVariable, variableLength);

    char *value = findValueInArrayByKey(variable);
    if (value == NULL) return NULL;

    char newCommand[64] = "";
    strncpy(newCommand, command, numOfCharsBeforeVariable-1);
    strcat(newCommand, value);
    strcat(newCommand, stringStartingWithVariable + variableLength +1);
    strcpy(command, newCommand);
  }

  // then scan for $ until next $ or space or EOL
  while (strstr(command, "$") != NULL) {
    int numOfCharsBeforeVariable = strcspn(command, "$");
    char stringStartingWithVariable[64] = "";
    strncpy(stringStartingWithVariable, command + numOfCharsBeforeVariable +1, strlen(command) - numOfCharsBeforeVariable -1);
    int variableLength = strcspn(stringStartingWithVariable, "$ ");
    if(variableLength == 0) variableLength = strlen(stringStartingWithVariable);

    char variable[64] = "";
    strncpy(variable, stringStartingWithVariable, variableLength);

    char *value = findValueInArrayByKey(variable);
    if (value == NULL) return NULL;

    char newCommand[64] = "";
    strncpy(newCommand, command, numOfCharsBeforeVariable);
    strcat(newCommand, value);
    strcat(newCommand, stringStartingWithVariable + variableLength);
    strcpy(command, newCommand);
  }
  return command;
}

char *findValueInArrayByKey(char *key) {
  char *value;
  for (int i = 0; i <= variableIndex; i++) {
    if(strcmp(storedVariables[i].key, key) == 0) {
      value = storedVariables[i].value;
    }
  }
  return value;
}
