#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "client.h"
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 6000

//Ahead function declaration for function use throughout main
int fexists(char* fileName);

int main() {
	//Welcoming user to program
	printf("Welcome to the Pokemon Query Client.\n");
	
	//Intializing socket variables
	int clientSocket;
	struct sockaddr_in serverAddress;
	int status, bytesRcv;
	//Intializing buffer to 300 to store large values
	char buffer[300];
	//Creating the client socket
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//Error code
	if (clientSocket < 0) {
		printf("Unable to establish connection to the PPS!.\n");
		exit(-1);
	}
	//Setting-up address
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP);
	serverAddress.sin_port = htons((unsigned short) SERVER_PORT);
	//Connecting to server
	status = connect(clientSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
	//Error code
	if (status < 0) {
		printf("Unable to establish connection to the PPS!\n");
		exit(-1);
	}
	
	//Initializing a dynamic array of Pokemon structs to store query results
	struct Pokemon* pokemonInMemory = NULL;
	int numOfPokemon = 1;
	
	//Creating a dynamic array to store new file names
	char** newFiles;
	int numNewFiles = 0;
	
	//Initialization of a variable to store the users choice
	int userChoice = 0;
	
	//Initialization of variable storing number of queries
	int numQueries = 0;
	
	//Loop to communicate with server, break condition as exit program choice
	while (userChoice != 3) {
		//Displaying menu options
		printf("MENU: Please type the number corresponding to your desired menu option.\n");
		printf("1. Type search\n");
		printf("2. Save results\n");
		printf("3. Exit the program\n");
	
		//Getting user choice
		scanf("%d", &userChoice);
		
		//Type search
		if (userChoice == 1) {
			//Prompting user for desired type1
			printf("Please print the type you'd like to search for.\n");
			char* userType = '\0';
			scanf("%ms", &userType);
			
			//Sending userType to server
			strcpy(buffer, userType);
			printf("CLIENT: Sending \"%s\" to server.\n", buffer);
			send(clientSocket, buffer, strlen(buffer), 0);
			//Loop to obtain server results
			while (1) {
				//Storing the results in buffer
				bytesRcv = recv(clientSocket, buffer, 300, 0);
				buffer[bytesRcv] = 0;
				char* token;
				//Using strtok to break the string around the commas
				token = strtok(buffer, ",");
				//Creating a temporary pokemon struct
				struct Pokemon tempPokemon;
				//Counter to determine which field is being set apart
				int counter = 0;
				while (token != NULL) {
					counter += 1;
					//Setting tempPokemon fields as necessary
					if (counter == 1) {
						tempPokemon.number = atoi(token);
					} else if (counter == 2) {
						strcpy(tempPokemon.name, token);
					} else if (counter == 3) {
						strcpy(tempPokemon.type1, token);
					} else if (counter == 4) {
						strcpy(tempPokemon.type2, token);
					} else if (counter == 5) {
						tempPokemon.total = atoi(token);
					} else if (counter == 6) {
						tempPokemon.HP = atoi(token);
					} else if (counter == 7) {
						tempPokemon.attack = atoi(token);
					} else if (counter == 8) {
						tempPokemon.defense = atoi(token);
					} else if (counter == 9) {
						tempPokemon.sp_atk = atoi(token);
					} else if (counter == 10) {
						tempPokemon.sp_def = atoi(token);
					} else if (counter == 11) {
						tempPokemon.speed = atoi(token);
					} else if (counter == 12) {
						tempPokemon.generation = atoi(token);
					} else if (counter == 13) {
						if (strcmp(token, "True") == 0) {
							tempPokemon.legendary = 1;
						} else {
							tempPokemon.legendary = 0;
						}
					}
					token = strtok(NULL, ",");
				}
				//Allocating memory to pokemonInMemory and saving tempPokemon to the array
				if (numOfPokemon == 1) {
					pokemonInMemory = calloc(1, sizeof(pokemon));
				} else {
					pokemonInMemory = realloc(pokemonInMemory, (numOfPokemon+1)*sizeof(pokemon));
				}
				pokemonInMemory[numOfPokemon - 1] = tempPokemon;
				numOfPokemon += 1;
				//Breaking out of the loop once message "end" is received
				if (strcmp("end", buffer) == 0) {
					break;
				}
			}
			//Incrementing the number of queries
			numQueries += 1;
		}
		
		//Saving results
		if (userChoice == 2) {
			FILE *fw;
			//Obtaining user appointed file name and determining whether it is valid
			printf("Please enter a filename to write to.\n");
			char* userFile = '\0';
			scanf("%ms", &userFile);
			int check = fexists(userFile);
			while (check) {
				printf("Unable to create the new file. Please enter the name of the file again.\n");
				scanf("%ms", &userFile);
				check = fexists(userFile);
			}
			//Opening file
			fw = fopen(userFile, "w");
			char writeLine[300];
			//Writing values from pokemonInMemory to file
			for (int i = 0; i < numOfPokemon; i++) {
				if (pokemonInMemory[i].legendary == 0) {
					fprintf(fw, "%d, %s, %s, %s, %d, %d, %d, %d, %d, %d, %d, %d, False\n", pokemonInMemory[i].number, pokemonInMemory[i].name, pokemonInMemory[i].type1, pokemonInMemory[i].type2, pokemonInMemory[i].total, pokemonInMemory[i].HP, pokemonInMemory[i].attack, pokemonInMemory[i].defense, pokemonInMemory[i].sp_atk, pokemonInMemory[i].sp_def, pokemonInMemory[i].speed, pokemonInMemory[i].generation);
				} else {
					fprintf(fw, "%d, %s, %s, %s, %d, %d, %d, %d, %d, %d, %d, %d, True\n", pokemonInMemory[i].number, pokemonInMemory[i].name, pokemonInMemory[i].type1, pokemonInMemory[i].type2, pokemonInMemory[i].total, pokemonInMemory[i].HP, pokemonInMemory[i].attack, pokemonInMemory[i].defense, pokemonInMemory[i].sp_atk, pokemonInMemory[i].sp_def, pokemonInMemory[i].speed, pokemonInMemory[i].generation);
				}
			}
			//Closing file
			fclose(fw);
			memset(pokemonInMemory, 0, sizeof(pokemonInMemory));
		}
	}
	
	//Providing program statistics
	printf("The total number of queries was %d.\n", numQueries);
	printf("The new files created during this program are:\n");
	for (int i = 0; i < numNewFiles; i++) {
		printf("%s\n", newFiles[i]);
		free(newFiles[i]);
	}
	printf("Thank you for using the Pokemon Query Program.\n");
	
	//Freeing dynamically allocated pointer
	free(pokemonInMemory);
	
	//Sending shutdown message to server
	char quit[] = "QuIt";
	strcpy(buffer, quit);
	send(clientSocket, buffer, strlen(buffer), 0);
	
	//Closing client socket
	close(clientSocket); // Don't forget to close the socket !
	printf("CLIENT: Shutting down.\n");
	
	return 0;
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
