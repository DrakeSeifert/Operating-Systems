#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>

void error(const char *msg) { fprintf(stderr,msg); exit(1); }
// Error function used for reporting issues

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	struct sockaddr_in serverAddress, clientAddress;

	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); }
	// Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress));
	// Clear out the address struct
	portNumber = atoi(argv[1]);
	// Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	// Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0){ fprintf(stderr,"ERROR opening socket"); exit(1);}

	// Enable the socket to begin listening
	if (bind(listenSocketFD,(struct sockaddr *)&serverAddress,sizeof(serverAddress)) < 0)
	// Connect socket to port
		{fprintf(stderr,"ERROR on binding");exit(1);}

	//int flag = 0;
	while(1)
    	{
		listen(listenSocketFD, 5);
		// Flip the socket on - it can now receive up to 5 connections
		// Accept a connection, blocking if one is not available until one connects
		sizeOfClientInfo = sizeof(clientAddress);
		// Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD<0) {fprintf(stderr,"ERROR on accept"); continue;}

		//create child process
		pid_t spawnpid = -5;

		spawnpid = fork();
		switch(spawnpid)
		{
			case -1:
				fprintf(stderr, "Error forking child\n");
				exit(1);
				break;
			case 0:
			{
				char buffer[1024];
				memset(buffer,'\0',sizeof(buffer));
				charsRead = recv(establishedConnectionFD, buffer, 1023, 0);
				if (charsRead < 0) error("ERROR reading from socket\n");

				/*
				int bufSize = atoi(buffer);

				char message[bufSize+1];
				memset(message,'\0',sizeof(message));
				//printf("TEST: sizeof(message) == %zu\n",sizeof(message));
				// Get the message from the client
				int i,j;
				do {
					memset(buffer, '\0', sizeof(buffer));
					charsRead = recv(establishedConnectionFD, buffer, 1024, 0);
					if (charsRead < 0) error("ERROR reading from socket\n");
					strcat(message,buffer);
				} while(buffer[1023] != '\0');
				printf("SERVER: I received this from the client: \"%s\"\n", message);
				*/
				
				if(sizeof(buffer) == 0) error("ERROR, empty message");
				int i,j;
				/*
				for(i=0;i<strlen(message);++i)
				{
					if(message[i] == 1)
						message[i] = ' ';
				}
				*/
				//check key length >= plaintext length
				char* token = 0;
				token = strtok(buffer, "\n");
				char* plaintext = token;
				//printf("TEST: plaintext == %s\n",plaintext);
				token = strtok(NULL, "\n");
				char* key = token;
				//printf("TEST: key == %s\n",key);
				
				if(strlen(key) < strlen(plaintext))
				{
						//error("Error! Key not long enough!\n");
						charsRead = send(establishedConnectionFD, "4", 1, 0);
						close(establishedConnectionFD);
						exit(1);
				}

				//Begin encryption, first check for valid input
				char myChars[] = {'A','B','C','D','E','F','G','H','I',
						  'J','K','L','M','N','O','P','Q','R',
						  'S','T','U','V','W','X','Y','Z',' ','\0'};
				
				int validInput = 0;
				for(i=0;i<strlen(plaintext);++i)
				{
					for(j=0;j<strlen(myChars);++j)
					{
						if(plaintext[i] == myChars[j])
						{
							validInput = 1;
							break;
						}
					}
					if(!validInput)
					{
						//error("Error! Bad chars in plaintext\n");
						charsRead = send(establishedConnectionFD, "5", 1, 0);
						close(establishedConnectionFD);
						exit(1);
					}
					else
						validInput = 0;
				}


				for(i=0;i<strlen(key);++i)
				{
					for(j=0;j<strlen(myChars);++j)
					{
						if(key[i] == myChars[j])
						{
							validInput = 1;
							break;
						}
					}
					if(!validInput){
						//error("Error! Bad chars in key file\n");
						fprintf(stderr,"Error! invalid input at key[%d] == %c == %d\n",i,key[i],key[i]);
						charsRead = send(establishedConnectionFD, "6", 1, 0);
						close(establishedConnectionFD);
						exit(1);
					}
					else
						validInput = 0;
				}

				int keyInt[strlen(plaintext)];
				for(i=0;i<strlen(plaintext);++i)
				{
					for(j=0;j<strlen(myChars);++j)
					{
						if(key[i] == myChars[j])
							keyInt[i] = j;
					}
				}				

				int numMsg[strlen(plaintext)];
				for(i=0;i<strlen(plaintext);++i)
				{
					for(j=0;j<strlen(myChars);++j)
					{
						if(plaintext[i] == myChars[j])
						{
							numMsg[i] = j;
						}
					}
				}

				int combine[strlen(plaintext)];
				int hash[strlen(plaintext)];
				for(i=0;i<strlen(plaintext);++i)
				{
					combine[i] = numMsg[i] + keyInt[i];
					if(combine[i] > 26)
						hash[i] = combine[i] - 26;
					else
						hash[i] = combine[i];
				}

				char hashInt[strlen(plaintext)+1];
				hashInt[strlen(plaintext)+1] = '\0';
				for(i=0;i<strlen(plaintext);++i)
				{
					hashInt[i] = myChars[hash[i]];
				}

				// Send a Success message back to the client
				/*
				char bufferBuf[1023];
				int k=0;
				do{
					memset(bufferBuf,'\0',sizeof(bufferBuf));
					for(i=0;i<sizeof(bufferBuf);++i)
					{
						if(k>=strlen(hashInt)) break;
						bufferBuf[i] = hashInt[k];
						k++;
					}
					printf("TEST: bufferBuf == \n%s\n",bufferBuf);
					charsRead = send(establishedConnectionFD, bufferBuf, strlen(bufferBuf), 0);
					if (charsRead < 0) error("ERROR writing to socket");
				}while(k<strlen(hashInt));
				*/

				charsRead = send(establishedConnectionFD, hashInt, sizeof(hashInt), 0);
				// Send success back
				if (charsRead < 0) error("ERROR writing to socket");
				close(establishedConnectionFD);
				//Close the existing socket which is connected to the client
	
				exit(0);
				break;
			}
			default:
				break;
		}
		//flag++;
		//if(flag > 10) exit(3); //prevent fork bomb
    	}
	close(listenSocketFD); // Close the listening socket
	return 0; 
}
