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

	if (argc < 4) { fprintf(stderr,"USAGE: %s ciphertext key port\n", argv[0]); exit(0); } // Check usage & args
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
		fprintf(stderr,"CLIENT: ERROR connecting");
		exit(2);
	}

	//append ciphertext and key to buffer array
	int fileSize=0;
	FILE *ciphertextFP;
	char *mybuf;
	ciphertextFP = fopen(argv[1], "r");
	if(!ciphertextFP) error("Invalid ciphertext file\n");
	
	fseek(ciphertextFP,0,SEEK_END);
	fileSize = ftell(ciphertextFP);
	fseek(ciphertextFP,0,SEEK_SET);
	rewind(ciphertextFP);
	mybuf = malloc((fileSize+1)*sizeof(*mybuf));
	fread(mybuf,fileSize,1,ciphertextFP);
	mybuf[fileSize] = '\0';
	fclose(ciphertextFP);

	fileSize = 0;
	FILE *keyFP;
	char *mybuf2;
	keyFP = fopen(argv[2],"r");
	if(!keyFP) error("Invalid key file\n");
	
	fseek(keyFP,0,SEEK_END);
	fileSize = ftell(keyFP);
	//printf("TEST: fileSize == %d\n",fileSize);
	fseek(keyFP,0,SEEK_SET);
	rewind(keyFP);
	mybuf2 = malloc((fileSize+1)*sizeof(*mybuf2));
	fread(mybuf2,fileSize,1,keyFP);
	mybuf2[fileSize] = '\0';
	fclose(keyFP);

	char *buffer;
	buffer = strcat(mybuf, mybuf2);
	//printf("TEST: buffer == \n%s",buffer);

	charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket\n");
	if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	/*
	int bufSizeInt = strlen(buffer);
	char bufSizeChar[1024];
	memset(bufSizeChar,'\0',sizeof(bufSizeChar));
	sprintf(bufSizeChar, "%d", bufSizeInt);

	// Send message size to server
	charsWritten = send(socketFD, bufSizeChar, strlen(bufSizeChar), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket\n");
	if (charsWritten < strlen(bufSizeChar)) printf("CLIENT: WARNING: Not all data written to socket!\n");
	//if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	//send message to server
	char bufferBuf[1023];
	int i,k=0;
	do{
		memset(bufferBuf,'\0',sizeof(bufferBuf));
		for(i=0;i<sizeof(bufferBuf);++i)
		{
			if(k>=bufSizeInt) break;
			bufferBuf[i] = buffer[k];
			k++;
		}
		//printf("TEST: bufferBuf == \n%s\n",bufferBuf);
		charsWritten = send(socketFD, bufferBuf, strlen(bufferBuf), 0); // Write to the server
		if (charsWritten < 0) error("CLIENT: ERROR writing to socket\n");
	}while(k<bufSizeInt);
	*/

	// Get return message from server
	char msgRecv[strlen(buffer)+1];
	memset(msgRecv,'\0',sizeof(msgRecv));
	charsRead = recv(socketFD, msgRecv, sizeof(msgRecv) - 1, 0);
	// Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket\n");

	if(!strcmp(msgRecv, "4")) error("Error: key too short\n");
	else if(!strcmp(msgRecv, "5")) error("Error: Invalid chars in ciphertext\n");
	else if(!strcmp(msgRecv, "6")) error("Error: Invalid chars in key file\n");
	
	printf("%s\n", msgRecv);
	//char finalMsg[strlen(msgRecv)+1];
	//memset(finalMsg,'\0',sizeof(finalMsg));
	//printf("%s", finalMsg);

	close(socketFD); // Close the socket
	return 0;
}
