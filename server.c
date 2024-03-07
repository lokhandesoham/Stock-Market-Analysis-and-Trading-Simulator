#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <math.h>

#define DELIMITERS " ,\n"
#define maxArgs 1024


typedef struct 
{
    char date[11];
    double close;
} serverLine;

void parse_csv(char *filename1, serverLine* firstCompanyArray)
{
    FILE *file = fopen(filename1, "r");
    if(file == NULL)
    {
        perror("Unable to open file");
        return;
    }

    char line[maxArgs];
    int row = 0;
    while(fgets(line, maxArgs, file))
    {
        char *token;
        int column = 0;


    

        token = strtok(line, ",");

        while (token)
        {
            if(row == 0)
            {
                row++;
                continue;
            }
            if(column == 0)
            {
                strcpy(firstCompanyArray[row].date, token);
            }

            if(column == 4)
            {


                firstCompanyArray[row].close = atof(token);
                //firstCompanyArray[row].close = round(firstCompanyArray[row].close * 100) / 100;


            }


            token = strtok(NULL, ",");
            column++;
        }
            row++;
        
    }
    fclose(file);
}

void execute_list(char *firstStockName, char *secondStockName)
{
    printf("%s | %s\n", firstStockName, secondStockName);
}


char * execute_price(char *date, serverLine *arrayOfStock)
{
  double price;
  static char msg[10] ;
  bool flag = false;
  for (int i = 0; i < maxArgs; i++) {
    if (strcmp(arrayOfStock[i].date, date) == 0 && strlen(arrayOfStock[i].date) > 0)
    {
        price = arrayOfStock[i].close;
        flag = true;
        break;
      
    } 
  }
  if (flag) {
        snprintf(msg, sizeof(msg), "%.2f", price);
    } else {
        strncpy(msg, "Unknown", sizeof(msg));
    }

    return msg;
}

double min(double x, double y) 
{
    return (x < y) ? x : y;
}

double max(double x, double y) 
{
    return (x > y) ? x : y;
}


char * execute_maxprofit(serverLine *arrayOfStock, char *startDate, char *endDate)
{
    bool startDateFound = false;
    int startDateInt;
    bool endDateFound = false;
    static char msg[10] ;

    for(int i = 0; i < maxArgs; i++)
    {
        if(strcmp(arrayOfStock[i].date, startDate) == 0)
        {
            startDateFound = true;
            startDateInt = i;
            break;
        }
    }

    if(!startDateFound)
    {
        //printf("Start date not found\n");
        strncpy(msg, "Unknown", sizeof(msg));
        return msg;
    }
    double minVal = arrayOfStock[startDateInt].close;
    double maxProfit = arrayOfStock[startDateInt + 1].close - arrayOfStock[startDateInt].close;

    for(int i = startDateInt + 1; i < maxArgs; i++)
    {
        if(arrayOfStock[i].close - minVal > maxProfit )
        {
            maxProfit = arrayOfStock[i].close - minVal;
        }
        if(arrayOfStock[i].close < minVal)
        {
            minVal= arrayOfStock[i].close;
        }
        if(strcmp(arrayOfStock[i].date, endDate) == 0)
        {
            endDateFound = true;
            break;
        }

    }

    if(!endDateFound)
    {
        //printf("End date not found\n");
        strncpy(msg, "Unknown", sizeof(msg));
        return msg;
    }

    snprintf(msg, sizeof(msg), "%.2f", maxProfit);
    return msg;
}





// void execute_commands(char *input[], int numTokens) {

//   if (strcmp(input[0], "realloc") == 0) {
//     int prevptr = atoi(input[1]);
//     int newSize = atoi(input[2]);
//     execute_realloc(prevptr, newSize);
//   }

//   else if (strcmp(input[0], "free") == 0) {
//     int pointer = atoi(input[1]);
//     execute_free(pointer);

//   }

//   else if (strcmp(input[0], "blocklist") == 0) {
//     execute_blocklist();
//   }
// }

serverLine firstArgvArray[maxArgs];
serverLine secondArgvArray[maxArgs];
char firstStockName[10];
char secondStockName[10];


int main(int argc, char *argv[]) {
    int port;
    
    if(argc == 4)
    {
      
      strncpy(firstStockName, argv[1], sizeof(firstStockName) - 1);
      strncpy(secondStockName, argv[2], sizeof(secondStockName) - 1);
      firstStockName[sizeof(firstStockName) - 1] = '\0';
      secondStockName[sizeof(secondStockName) - 1] = '\0';
      parse_csv(firstStockName, firstArgvArray);
      parse_csv(secondStockName, secondArgvArray);
      firstStockName[sizeof(firstStockName) - 6] = '\0';
      secondStockName[sizeof(secondStockName) - 6] = '\0';
      port = atoi(argv[3]);
    }
    else if(argc == 3)
    {
  
      strncpy(firstStockName, argv[1], sizeof(firstStockName) - 1);
      firstStockName[sizeof(firstStockName) - 1] = '\0';
      parse_csv(firstStockName, firstArgvArray);
      firstStockName[sizeof(firstStockName) - 6] = '\0';
      port = atoi(argv[2]);
    }
    else
    {
      printf("No arguments given");
    return 1; 
    }

    
    
   
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[256];
    int n;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0){
    perror("Socket error");
    exit(1);
    }
    printf("server started\n");

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    n = bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (n < 0){
    perror("Bind error");
    exit(1);
    }

    listen(server_socket, 1);

    addr_size = sizeof(client_addr);
    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);
    
    while(1)
    {
      char *tokenizedInput[256];
      int counter = 0;
      bzero(buffer, 256);
      recv(client_socket, buffer, sizeof(buffer), 0);

      char *token = strtok(buffer, DELIMITERS);
      while (token != NULL && counter < 256) {
        tokenizedInput[counter++] = strdup(token);
        token = strtok(NULL, DELIMITERS);
      }


      if (strncmp(tokenizedInput[1], "quit",4) == 0) {
        close(client_socket);
        close(server_socket);
        exit(0);
      }
      else if(strncmp(tokenizedInput[1], "List",4) == 0) {
        bzero(buffer, 256);
        char info[1024];
        if(argc == 4)
          sprintf(info, "%s | %s", firstStockName, secondStockName);
        else
          sprintf(info, "%s", firstStockName);

        sprintf(buffer, "%ld ", sizeof(info));
        strcat(buffer, info);
      }
      else if(strncmp(tokenizedInput[1], "Prices",6) == 0) {
        bzero(buffer, 256);
        char price[10];

        if(strcmp(tokenizedInput[2],firstStockName)==0)
          strcpy(price, execute_price(tokenizedInput[3],firstArgvArray));
        else if(strcmp(tokenizedInput[2],secondStockName)==0)
          strcpy(price, execute_price(tokenizedInput[3],secondArgvArray));

        sprintf(buffer, "%ld ", sizeof(price));
        strcat(buffer, price);

        bzero(price, 10);
        
      }
      else if(strncmp(tokenizedInput[1], "MaxProfit",9) == 0) {
        bzero(buffer, 256);
        char price[10];

        if(strcmp(tokenizedInput[2],firstStockName)==0)
          strcpy(price, execute_maxprofit(firstArgvArray,tokenizedInput[3],tokenizedInput[4]));
        else if(strcmp(tokenizedInput[2],secondStockName)==0)
          strcpy(price, execute_maxprofit(secondArgvArray,tokenizedInput[3],tokenizedInput[4]));

        sprintf(buffer, "%ld ", sizeof(price));
        strcat(buffer, price);

        bzero(price, 10);
        
      }



      for(int i = 1; i < counter; i++)
      {
        printf("%s ", tokenizedInput[i]);
      }
      printf("\n");

      

      send(client_socket, buffer, strlen(buffer), 0);
      
    }

    return 0;
}

