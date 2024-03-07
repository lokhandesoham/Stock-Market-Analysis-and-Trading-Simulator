#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>

#define MAX_ARGS_N 1098
#define DELIMITERS " ,\n"

// char* execute_commands(char *input[], int numTokens) {
//   if (strcmp(input[0], "List") == 0) {
//     int numberOfBytes = atoi(input[1]);
//     execute_malloc(numberOfBytes);
//   }

//   else if (strcmp(input[0], "realloc") == 0) {
//     int prevptr = atoi(input[1]);
//     int newSize = atoi(input[2]);
//     execute_realloc(prevptr, newSize);
//   }

//   else if (strcmp(input[0], "free") == 0) {
//     int pointer = atoi(input[1]);
//     execute_free(pointer);
//   }
// }

bool date_format_check(char* date)
{
    if (strlen(date) != 10) {
        return false;  
    }

    if (date[4] != '-' || date[7] != '-') {
        return false; 
    }

    for (int i = 0; i < 10; i++) {
        if (i != 4 && i != 7 && (date[i] < '0' || date[i] > '9')) {
            return false;  
        }
    }
    
    if( date[5] > '1')
          return false;
    if( date[5] >'0' && date[6] > '2')
          return false;
    if( date[8]> '2' && date[9] > '1')
          return false;
    if( (date[5] < '1' && date[6] < '1') || (date[8] < '1' && date[9] < '1'))
          return false;

    return true;  
}

  int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("No arguments given");
        return 1;  
    }

    char *server_name = argv[1];
    int port = atoi(argv[2]);
   

    int client_socket;
    struct sockaddr_in addr;
    socklen_t addr_size;
    int n;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
    perror("Socket error");
    exit(1);
    }

    

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    struct hostent* host = gethostbyname(server_name);
    addr.sin_addr = *(struct in_addr*)host->h_addr_list[0];

    if (connect(client_socket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    perror("Connect error");
    exit(1);
    }
    

    char buffer[256];
    bzero(buffer, 256);
  
    while(1)
    {
      
      printf("> ");
      char *tokenizedInput[256] = {NULL};
      int counter = 0;
      char input[256];
      fgets(input, sizeof(input), stdin);
      char dup_input[256];
      strcpy(dup_input, input);

      char *token = strtok(input, DELIMITERS);
      while (token != NULL && counter < 256) {
        tokenizedInput[counter++] = strdup(token);
        token = strtok(NULL, DELIMITERS);
      }


      bool flag=false;
      if(strcmp(tokenizedInput[0], "Prices") == 0)
         flag = date_format_check(tokenizedInput[2]);
      
      else if(strcmp(tokenizedInput[0], "MaxProfit") == 0)
        flag = date_format_check(tokenizedInput[2]) && date_format_check(tokenizedInput[3]);
      
      else if(strcmp(tokenizedInput[0], "List") == 0)
        flag = true;
      else if(strcmp(tokenizedInput[0], "quit") == 0)
        flag = true;
      


      if(flag)
      {
        sprintf(buffer, "%ld ", sizeof(dup_input));
        strcat(buffer, dup_input);
        send(client_socket, buffer, strlen(buffer), 0);
      }
      else
      {
        printf("Invalid syntax.\n");
        continue;
      }

      if(strncmp(tokenizedInput[0], "quit",4) == 0)
      {
        close(client_socket);
        exit(0);
      }



      bzero(buffer, 256);
      recv(client_socket, buffer, sizeof(buffer), 0);

      char *tokenizedInput2[256] = {NULL};
      int counter2 = 0;

      char *token2 = strtok(buffer, DELIMITERS);
      while (token2 != NULL && counter2 < 256) {
        tokenizedInput2[counter2++] = strdup(token2);
        token2 = strtok(NULL, DELIMITERS);
      }

      for(int i = 1; i < counter2; i++)
      {
        printf("%s ", tokenizedInput2[i]);
      }
      printf("\n");
    }

    return 0;

}



