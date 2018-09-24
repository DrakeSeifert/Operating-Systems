#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[])
{
	if(argc != 2){fprintf(stderr,"USAGE: %s keyLength\n", argv[0]); exit(1);}

	int keylength = atoi(argv[1]);

	int i;
	time_t t;
	srand((unsigned)time(&t));

	char myChars[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N',
				'O','P','Q','R','S','T','U','V','W','X','Y','Z',' '};

	int randNum;
	char mykey[keylength];
	memset(mykey,'$',sizeof(mykey));
	for(i=0;i<sizeof(mykey);++i)
	{
		randNum = rand()%28;
		mykey[i] = myChars[randNum];
	}
	
	i=0;
	int count = 0;
	while(mykey[i] != '$' && i<sizeof(mykey)) 
	{
		if(mykey[i] == '\0') mykey[i] = ' ';
		i++;
		count++;
	}
	mykey[i] = '\0';

	printf("%s",mykey);
	printf("\n");

	return 0;
}
