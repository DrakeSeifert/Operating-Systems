#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg) { fprintf(stderr,msg); exit(1); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	//char buffer[256];
	//memset(buffer, '\0', sizeof(buffer));

	if (argc < 4) { fprintf(stderr,"USAGE: %s plaintext key port\n", argv[0]); exit(0); } // Check usage & args

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);
	// Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");
	
	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
	// Connect socket to address
	{
		fprintf(stderr,"CLIENT: ERROR connecting\n");
		exit(2);
	}

	// Get input message from user:

	//append plaintext and key to buffer array
	int fileSize=0;
	FILE *plaintextFP;
	char *mybuf;
	plaintextFP = fopen(argv[1], "r");
	if(!plaintextFP) error("Invalid plaintext file\n");
	
	fseek(plaintextFP,0,SEEK_END);
	fileSize = ftell(plaintextFP);
	fseek(plaintextFP,0,SEEK_SET);
	rewind(plaintextFP);
	mybuf = malloc((fileSize+1)*sizeof(*mybuf));
	fread(mybuf,fileSize,1,plaintextFP);
	mybuf[fileSize] = '\0';
	fclose(plaintextFP);	

	int fileSize2 = 0;
	FILE *keyFP;
	char *mybuf2;
	keyFP = fopen(argv[2],"r");
	if(!keyFP) error("Invalid key file\n");
	
	fseek(keyFP,0,SEEK_END);
	fileSize2 = ftell(keyFP);
	//printf("TEST: fileSize2 == %d\n",fileSize2);
	fseek(keyFP,0,SEEK_SET);
	rewind(keyFP);
	mybuf2 = malloc((fileSize2+1)*sizeof(*mybuf2));
	fread(mybuf2,fileSize2,1,keyFP);
	mybuf2[fileSize2] = '\0';
	fclose(keyFP);
	//printf("TEST: mybuf2 == \n%s\n",mybuf2);

	char *buffer;
	buffer = strcat(mybuf, mybuf2);
	//printf("TEST: buffer == %s\n", buffer);
	
	charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket\n");
	if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");
	int i,j;
	/*
	//send buffer size to server
	int bufSizeInt = strlen(buffer);
	char bufSizeChar[1024];
	memset(bufSizeChar,'\0',sizeof(bufSizeChar));
	sprintf(bufSizeChar, "%d", bufSizeInt);
	//printf("TEST: bufSizeInt == %d\n",bufSizeInt);
	//printf("TEST: bufSizeChar == %s\n",bufSizeChar);

	//charsWritten = send(socketFD, bufSizeChar, strlen(bufSizeChar), 0); // Write to the server
	charsWritten = send(socketFD, bufSizeChar, strlen(bufSizeChar), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket\n");
	if (charsWritten < strlen(bufSizeChar)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	// Send message to server
	char bufferBuf[1024];
	int i,k = 0;
	do {
		memset(bufferBuf,'\0',sizeof(bufferBuf));
		for(i=0;i<sizeof(bufferBuf);++i)
		{
			if(k >= bufSizeInt) break;
			bufferBuf[i] = buffer[k];
			//printf("TEST: buffer[%d] == %c\n",k,buffer[k]);
			k++;
		}
		printf("TEST: bufferBuf == %s\n",bufferBuf);
		charsWritten = send(socketFD, bufferBuf, strlen(bufferBuf), 0); // Write to the server
		if (charsWritten < 0) error("CLIENT: ERROR writing to socket\n");
		//if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");
	}while(k<bufSizeInt);
	*/

	// Get return message from server
	
	char msgRecv[strlen(buffer)+1];
	memset(msgRecv,'\0',sizeof(msgRecv));
	/*
	char buffer2[1024];
	
	do{
		memset(buffer2,'\0',sizeof(buffer2));
		//charsRead = recv(socketFD, buffer2, 1024, 0);
		charsRead = recv(socketFD, buffer2, sizeof(buffer2)-1, 0);
		if (charsRead < 0) error("CLIENT: ERROR reading from socket\n");
		strcat(msgRecv,buffer2);
	}while(buffer[1023] != '\0');

	int j;
	for(i=0;i<strlen(msgRecv);++i)
	{
		if(msgRecv[i] == 1)
			msgRecv[i] = ' ';
	}
	*/

	charsRead = recv(socketFD, msgRecv, sizeof(msgRecv) - 1, 0);
	// Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket\n");

	//Check for error messages
	if(!strcmp(msgRecv, "4")) error("Error: key too short\n");
	else if(!strcmp(msgRecv,"5")) error("Error: Invalid chars in plaintext\n");
	else if(!strcmp(msgRecv,"6")) error("Error: Invalid chars in key file\n");
	
	char validChars[] = {'A','B','C','D','E','F','G','H','I','J','K',
				'L','M','N','O','P','Q','R','S','T','U',
				'V','W','X','Y','Z',' ','\0'};

	char finalMsg[strlen(msgRecv)+1];
	memset(finalMsg,'\0',sizeof(finalMsg));

	int isValid = 0;
	for(i=0;i<sizeof(finalMsg);++i)
	{
		for(j=0;j<strlen(validChars);++j)
		{
			if(msgRecv[i] == validChars[j])
			{
				finalMsg[i] = msgRecv[i];
				isValid = 1;
				break;
			}
		}
		if(!isValid) break;
		else isValid = 0;
	}
	//finalMsg[strlen(msgRecv)] = '\n';
	printf("%s\n", finalMsg);

	close(socketFD); // Close the socket
	return 0;
}
