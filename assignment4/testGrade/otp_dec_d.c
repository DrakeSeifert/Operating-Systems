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
	char buffer[256];
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

	int flag = 0;
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
				//TODO verify connected with otp_enc and not otp_dec
				//printf("Greetings from child process!\n");
				memset(buffer, '\0', 256);
				// Get the message from the client
				charsRead = recv(establishedConnectionFD, buffer, 255, 0);
				if (charsRead < 0) error("ERROR reading from socket");
				//printf("SERVER: I received this from the client: \"%s\"\n", buffer);

				//check key length >= ciphertext length
				char* token = 0;
				token = strtok(buffer, "\n");
				char* ciphertext = token;
				//printf("TEST: ciphertext == %s\n",ciphertext);
				token = strtok(NULL, "\n");
				char* key = token;
				//printf("TEST: key == %s\n",key);
				
				//int count = 0;
				int i,j;
				/*
				for(i=0;i<strlen(key);++i)
				{
					if(key[i] == ' ') count++;
				}
				printf("TEST: count == %d\n",count);
				printf("TEST: strlen(ciphertext) == %zu\n", strlen(ciphertext));
				*/
				//if(count < strlen(ciphertext))
				if(strlen(key) < strlen(ciphertext))
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
				for(i=0;i<strlen(ciphertext);++i)
				{
					for(j=0;j<strlen(myChars);++j)
					{
						if(ciphertext[i] == myChars[j])
						{
							validInput = 1;
							break;
						}
					}
					if(!validInput)
					{
						//error("Error! Bad chars in ciphertext\n");
						charsRead = send(establishedConnectionFD, "5", 1, 0);
						close(establishedConnectionFD);
						exit(1);
					}
					else
						validInput = 0;
				}

				//char myInts[] = {'0','1','2','3','4','5','6','7','8','9',
				//		 ' ','\0'};

				for(i=0;i<strlen(key);++i)
				{
					//for(j=0;j<strlen(myInts);++j)
					for(j=0;j<strlen(myChars);++j)
					{
						//if(key[i] == myInts[j])
						if(key[i] == myChars[j])
						{
							validInput = 1;
							break;
						}
					}
					if(!validInput)
					{
						//error("Error! Bad chars in key file\n");
						charsRead = send(establishedConnectionFD, "6", 1, 0);
						close(establishedConnectionFD);
						exit(1);
					}
					else
						validInput = 0;
				}

				/*
				char* temp[strlen(ciphertext)];
				token = strtok(key, " ");
				temp[0] = token;
				for(i=1;i<strlen(ciphertext);++i)
				{
					token = strtok(NULL, " ");
					temp[i] = token;
				}
				
				int keyInt[strlen(ciphertext)];
				j=0;
				for(i=0;i<strlen(ciphertext);++i)
				{
					keyInt[j] = atoi(temp[i]);
					j++;
				}
				*/
				
				//decrypt ciphertext
				
				int keyInt[strlen(ciphertext)];
				for(i=0;i<strlen(ciphertext);++i)
				{
					for(j=0;j<strlen(myChars);++j)
					{
						if(key[i] == myChars[j])
							keyInt[i] = j;
					}
				}

				int decChar[strlen(ciphertext)]; //decrypt chars to ints
				for(i=0;i<strlen(ciphertext);++i)
				{
					for(j=0;j<strlen(myChars);++j)
					{
						if(ciphertext[i] == myChars[j])
						{
							decChar[i] = j;
						}
					}
				}

				int uncombine[strlen(ciphertext)]; //find num for key[i]+numMsg[i]
				int numMsg2[strlen(ciphertext)]; //subtract key from uncombine to get msg int
				for(i=0;i<strlen(ciphertext);++i)
				{
					if((decChar[i] - keyInt[i]) <= 0)
						uncombine[i] = decChar[i] + 26;
					else
						uncombine[i] = decChar[i];
					numMsg2[i] = uncombine[i] - keyInt[i];
				}

				char decrypt[strlen(ciphertext)];
				for(i=0;i<strlen(ciphertext);++i)
				{
					decrypt[i] = myChars[numMsg2[i]];
				}
				
				//TEST
				for(i=0;i<strlen(ciphertext);++i)
				{
					printf("decrypt[%d] == %c\n",i,decrypt[i]);
				}

				// Send a Success message back to the client
				charsRead = send(establishedConnectionFD, decrypt, sizeof(decrypt), 0);
				// Send success back
				if (charsRead < 0) error("ERROR writing to socket");
				close(establishedConnectionFD);
				//Close the existing socket which is connected to the client
	
				exit(0);
				break;
			}
			default: //FIXME need this?
				//printf("Greetings from parent process!\n");
				break;
		}
		flag++;
		if(flag > 10) exit(3); //prevent fork bomb
    	}
	close(listenSocketFD); // Close the listening socket
	return 0; 
}
