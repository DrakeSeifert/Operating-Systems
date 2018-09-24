#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main()
{
	time_t t;
	srand((unsigned) time(&t));

	char myChars[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',
			    'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 
			    'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' ', '\0'};
	char message[] = {'H', 'E', 'L', 'L', 'O', ' ', 'W', 'O', 'R', 'L', 'D', '\0'};
	int key[256];
	memset(key, '\0', sizeof(key));
	

	int i,j;

	for(i=0;i<strlen(message);++i)
	{
		key[i] = rand() % 28;
	}

	int numMsg[strlen(message)]; //convert chars to ints
	printf("strlen(message) == %zu\n",strlen(message));
	for(i=0;i<strlen(message);++i)
	{
		for(j=0;j<strlen(myChars);++j)
		{
			if(message[i] == myChars[j])
			{
				numMsg[i] = j; //assigned int is array location of char
			}
		}
	}

	for(i=0;i<strlen(message);++i)
	{
		printf("numMsg[%d] == %d\n", i, numMsg[i]);
	}

	for(i=0;i<strlen(message);++i)
	{
		printf("key[%d] == %d\n", i, key[i]);
	}

	int combine[strlen(message)]; //add numMsg and key together
	int hash[strlen(message)]; //account for mod 26
	for(i=0;i<strlen(message);++i)
	{
		combine[i] = numMsg[i] + key[i];
		if(combine[i] > 26)
			hash[i] = combine[i] - 26;
		else
			hash[i] = combine[i];
	}

	for(i=0;i<strlen(message);++i)
	{
		printf("combine[%d] == %d\n", i, combine[i]);
	}

	for(i=0;i<strlen(message);++i)
	{
		printf("hash[%d] == %d\n", i, hash[i]);
	}

	char hashInt[strlen(message)+1]; //convert mod 26 addition of numMsg and key to chars
	hashInt[strlen(message)+1] = '\0';
	for(i=0;i<strlen(message);++i)
	{
		hashInt[i] = myChars[hash[i]];
	}

	for(i=0;i<strlen(message);++i)
	{
		printf("hashInt[%d] == %c\n", i, hashInt[i]);
	}

	printf("Beginning decrypt!\n");
	
	int decChar[strlen(message)]; //decrypt chars to ints
	for(i=0;i<strlen(message);++i)
	{
		for(j=0;j<strlen(myChars);++j)
		{
			if(hashInt[i] == myChars[j])
			{
				decChar[i] = j;
			}
		}
	}	

	for(i=0;i<strlen(message);++i)
	{
		printf("decChar[%d] == %d\n",i,decChar[i]);
	}

	int uncombine[strlen(message)]; //find for at key[i]+numMsg[i]
	int numMsg2[strlen(message)]; //subtract key from uncombine to get msg ints
	for(i=0;i<strlen(message);++i)
	{
		if((decChar[i] - key[i]) <= 0)
			uncombine[i] = decChar[i] + 26;
		else
			uncombine[i] = decChar[i];
		numMsg2[i] = uncombine[i] - key[i];
	}	

	for(i=0;i<strlen(message);++i)
	{
		printf("uncombine[%d] == %d\n",i,uncombine[i]);
	}
	
	for(i=0;i<strlen(message);++i)
	{
		printf("numMsg2[%d] == %d\n",i,numMsg2[i]);
	}
	
	/*
	int numMsg2[strlen(message)];
	for(i=0;i<strlen(message);++i)
	{
		numMsg2[i] = uncombine[i] - key[i];
	}
	*/

	char decrypt[strlen(message)]; //finally, convert numMsg2 to chars
	for(i=0;i<strlen(message);++i)
	{
		decrypt[i] = myChars[numMsg2[i]];
	}
	
	for(i=0;i<strlen(message);++i)
	{
		printf("decrypt[%d] == %c\n",i,decrypt[i]);
	}
	
	return 0;
}
