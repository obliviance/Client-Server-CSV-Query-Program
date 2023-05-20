#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "server.h"
#define SERVER_PORT 6000

//Ahead function declaration for function use throughout main
int fexists(char* fileName);

int main() {
 	//Creation of a FILE object for storage of pokemon.csv file pointer
	FILE *fd;
	//Prompting user for filename
	printf("Welcome to the Pokemon Property Server.\n");
	printf("Please type the name of the CSV file containing information on Pokemon.\n");
	//Initialization of filename variable
	char* fileName = '\0';
	//Obtaining user filename
	scanf("%ms", &fileName);
	//Initialization of check variable using the fexists function to determine whether the file exists
	int check = fexists(fileName);
	//Input validation loop
	while (!check) {
		//Error message
		printf("Pokemon file is not found. Please enter the name of the file again, or enter 'quit' to end the program.\n");
		//Obtaining input
		scanf("%ms", &fileName);
		//Check variable and fexists call
		check = fexists(fileName);
		//Initialization of QUIT const for definition of "quit"
		const char QUIT[] = "quit";
		//Processing a quit response
		if (strcmp(fileName, QUIT) == 0) {
			printf("Thank you for using the Pokemon Property Server.");
			return 0;
		}
	}
	
	//Initializing socket variables
	int serverSocket, clientSocket;
	struct sockaddr_in serverAddress, clientAddr;
	int status, addrSize, bytesRcv;
	char buffer[30];

	//Creating the server socket
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//Error coding
	if (serverSocket < 0) {
		printf("*** SERVER ERROR: Could not open socket.\n");
		exit(-1);
	}
	//Setting-up the server address
	memset(&serverAddress, 0, sizeof(serverAddress)); // zeros the struct
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons((unsigned short) SERVER_PORT);
	//Binding the server socket
	status = bind(serverSocket, (struct sockaddr *)&serverAddress,
	sizeof(serverAddress));
	//More error coding
	if (status < 0) {
		printf("*** SERVER ERROR: Could not bind socket.\n");
		exit(-1);
	}

	//Set up the line-up to handle up to 5 clients in line
	status = listen(serverSocket, 5);
	if (status < 0) {
		printf("*** SERVER ERROR: Could not listen on socket.\n");
		exit(-1);
	}
	//Loop for waiting for clients
	while (1) {
		//Connecting to client socket
		addrSize = sizeof(clientAddr);
		clientSocket = accept(serverSocket,(struct sockaddr *)&clientAddr,&addrSize);
		//Even more coding
		if (clientSocket < 0) {
			printf("*** SERVER ERROR: Could accept incoming client connection.\n");
			exit(-1);
		}
		//Infinite loop to talk to client
		while (1) {
			//Obtaining the message from client
			bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
			buffer[bytesRcv] = 0; // put a 0 at the end so we can display the string
			printf("SERVER: Received client request: %s\n", buffer);
			//Initialization of variable to hold one line's contents of the CSV file
			char fileLine[300];
			char fileLine2[300];
			fd = fopen(fileName, "r");
			//Using fgets to go through the file line by line
			while (fgets(fileLine, 300, fd)) {
				//Making a duplicate so that strtok does not get rid of the string
				strcpy(fileLine2, fileLine);
				char* token;
				//Using strtok to break the string around delimiters
				token = strtok(fileLine, ",");
				int counter = 0;
				while (token != NULL) {
					counter += 1;
					//Checking if the type1 in file is equivalent to the user type specified
					if ((counter == 3) && strcmp(token, buffer) == 0) {
						//Sending the file line to client
						send(clientSocket, fileLine2, strlen(fileLine2), 0);
						//Small delay in order to enable client to accept transfer
						usleep(10);
					}
					token = strtok(NULL, ",");
				}
			}
			fclose(fd);
			//Sending end of transfer message
			printf("SERVER: Sending \"%s\" to client\n", "end");
			send(clientSocket, "end", strlen("end"), 0);
			if (strcmp(buffer, "QuIt") == 0) {
				break;
			}
		}
		
		if (strcmp(buffer, "QuIt") == 0) {
				break;
		}
		printf("SERVER: Closing client connection.\n");
		close(clientSocket); // Close this client's socket
	}
	//Closing socket
	close(serverSocket);
	printf("SERVER: Shutting down.\n");
}

//Function definition
int fexists(char *fileName) {

	FILE *fp = NULL;
	fp = fopen(fileName, "r");

	if (fp) {
		fclose(fp);
		return(1);
	}
	
	return(0);
}
