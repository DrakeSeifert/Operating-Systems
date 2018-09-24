#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

void catchSIGINT(int signo)
{
	const char* message = "\nTerminated by signal 2\n";
	write(STDOUT_FILENO, message, 24);
}

void catchSIGTERM(int signo)
{
	const char* message = "\nTerminated by signal 15\n";
	write(STDOUT_FILENO, message, 25);
}

//print and flush
void paf(const char toPrint[2048])
{
	fprintf(stdout, toPrint);
	fflush(stdout);
}

int countStrings(char** myStringArray)
{
	int countWords = 0;
	int i=0;
	
	while(myStringArray[i] != '\0')
	{
		countWords++;
		i++;
	}
	return countWords;
}

char** splitSpaces(char splitMe[2048])
{
	int i;
	char** myWord = (char**)malloc(512*sizeof(char*)); //words
	for(i=0;i<512;++i)
	{
		myWord[i] = (char*)malloc(2048*sizeof(char));
		myWord[i] = '\0';
	}

	char* token = 0;
	token = strtok(splitMe, " ");
	i = 0;
	while(token)
	{
		myWord[i] = token;
		token = strtok(NULL, " ");
		++i;
	}

	return myWord;
}

int checkEmptyInput(char userInput[2048])
{ return userInput[0] == '\0'; }

int checkComment(char userInput[2048])
{ return userInput[0] == '#'; }

void currentStatus(int status, int badExit)
{
	if(badExit)
	{
		if(status == 256)
			status = 1;
		fprintf(stdout, "exit value %d\n",status);
		fflush(stdout);
	}
	else if(WIFEXITED(status))
	{
		if(status == 256)
			status = 1;	
		fprintf(stdout,"exit value %d\n",status);
		fflush(stdout);
	}
	else if(WIFSIGNALED(status))
	{
		fprintf(stdout,"Child process ended by signal status: %d\n", status);
		fflush(stdout);
	}
	else if(WIFSTOPPED(status))
	{	paf("TEST: Child process has been stopped\n");}
}

void checkFinishedChild(pid_t childPIDArray[512], int status)
{
	pid_t tempArray[512], endID;
	memset(tempArray, '\0', sizeof(tempArray));
	int i;
	int j = 0;

	for(i=0;i<512;++i)
	{
		if(childPIDArray != '\0')
		{
			tempArray[j] = childPIDArray[i];
			j++;
		}
	}

	for(i=0;i<512;++i)
	{
		childPIDArray[i] = tempArray[i];
	}

	for(i=0;i<512;++i)
	{
		if(childPIDArray[i] != '\0')
		{
			endID = waitpid(childPIDArray[i], &status, WNOHANG | WUNTRACED);
			if(endID == -1)
				{perror("Error in parent\n"); exit(1);}
			else if(endID)
			{
				fprintf(stdout,"Child with PID %d has finished: ",endID);
				fflush(stdout);
				childPIDArray[i] = '\0';
				currentStatus(status, 0);
			}
		}
		
	}
}

void promptUser(char userInput[2048], pid_t childPIDArray[512], int status)
{
	do
	{
		memset(userInput, '\0', sizeof(userInput));
		checkFinishedChild(childPIDArray, status);

		paf(": ");
		fscanf(stdin,"%[^\n]%*c", userInput);

		//clear input buffer for empty input
		if(checkEmptyInput(userInput))
		{
			getchar();
			fflush(stdin);
		}

	} while(checkEmptyInput(userInput) || checkComment(userInput));
}

void changeDirectories(char userInput[2048])
{
	//Go to specified directory
	if(!strncmp(userInput, "cd ", 3))
	{
		char myDir[2048];
		memset(myDir, '\0', sizeof(myDir));

		int i = 3;
		while(userInput[i] != '\0')
		{
			myDir[i-3] = userInput[i];
			++i;
		}

		//check if entered directory exists
		if(chdir(myDir) != -1)
		{
			chdir(getcwd(myDir, sizeof(myDir)));
			setenv("PWD", getcwd(myDir, sizeof(myDir)), 1);
		}
		else paf("Error: Invalid directory\n");
	}
	//else go to HOME directory
	else if(!strncmp(userInput, "cd\0", 3))
	{
		chdir(getenv("HOME"));
		setenv("PWD", getenv("HOME"), 1);
	}
	else paf("Error in changeDirectories() function\n");
}

//If built in command entered, performs command.
//Else return 0
int checkBuiltInCommand(char userInput[2048], int status, int badExit)
{
	//exit
	if(!strcmp(userInput, "exit"))
	{
		exit(0);
	}

	//cd
	else if(!strncmp(userInput, "cd ", 3) || !strncmp(userInput, "cd\0", 3))
	{
		changeDirectories(userInput);
	}

	//status
	else if(!strcmp(userInput, "status"))
	{
		currentStatus(status, badExit);
	}

	//non-built in command received
	else
		return 0;

	return 1;
}

void beginChildProcess(char** splitString,char* optArgs[512],int *status,char* inFile,char* outFile,int isBP, pid_t *childPIDArray)
{	
	pid_t spawnpid = -5, endID;
	int targetFD, sourceFD, result;

	spawnpid = fork();
	switch(spawnpid)
	{
		case -1: //Error
			perror("Hull Breach!");
			exit(1);
			break;

		case 0: //Child
			if(isBP) //redirect stdin and stdout to /dev/null for background processes
			{
				int saveOut = dup(STDOUT_FILENO);
				if(saveOut == -1) {perror("Error!"); exit(1);}
				int devNull = open("/dev/null", O_WRONLY);
				if(devNull == -1) {perror("Error!"); exit(1);}
				int dup2Result = dup2(devNull, STDOUT_FILENO);
				if(dup2Result == -1) {perror("Error!"); exit(1);}
				
				int saveIn = dup(STDIN_FILENO);
				if(saveIn == -1) {perror("Error!"); exit(1);}
				int devNull2 = open("/dev/null", O_RDWR);
				if(devNull2 == -1) {perror("Error!"); exit(1);}
				int dup2Result2 = dup2(devNull, STDIN_FILENO);
				if(dup2Result2 == -1) {perror("Error!"); exit(1);}
			}
			if(inFile != '\0' && !strcmp(splitString[0],inFile))
			{
				targetFD = open(outFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
				if(targetFD == -1)
				{
					paf("ERROR1\n");
					(*status) = 1;
					exit(1);}
				result = dup2(targetFD, 1); //stdout goes to targetFD's stdin
				if(execvp(optArgs[0], optArgs) == -1)
				{
					paf("ERROR2\n");
					(*status) = 1;
					exit(1);}
			}
			else if(outFile != '\0' && !strcmp(splitString[0],outFile))
			{
				sourceFD = open(inFile, O_RDONLY);
				if(sourceFD == -1)
				{
					paf("ERROR3\n");
					(*status) = 1;
					exit(1);}
				result = dup2(sourceFD, 0); //input comes from sourceFD
				if(execvp(optArgs[0],optArgs) == -1)
				{
					paf("ERROR4\n");
					(*status) = 1;
					exit(1);}
			}
			//else no input/output redirection
			else
			{
				if(execvp(splitString[0], optArgs) == -1)
				{
					perror("Error");
					(*status) = 1;
					exit(1);
				}
			}
			exit(3);
			break;

		default: //Parent
			if(isBP)
			{
				printf("Background PID is %d\n",spawnpid);
				//Put child PID in array
				int i;
				for(i=0;i<512;++i)
				{
					if(childPIDArray[i] == '\0')
					{
						childPIDArray[i] = spawnpid;
						break;
					}
				}
			}
			else
			{
				endID = waitpid(spawnpid, status, 0);
			}
			break;
	}	
}

//organize Command Line:
//sort optional args into array and prepare for stdout/stdin redirection
int organizeCL(char** splitString, int *status, pid_t *childPIDArray)
{
	int isBP = 0; //isBackgroundProcess

	char* optArgs[512];
	memset(optArgs, '\0', sizeof(optArgs));
	int i;
	int optArgsIndex = 1;

	char* inFile = '\0'; //From inFile
	char* outFile = '\0'; //to outFile. Ex: inFile > outFile or outFile < inFile
	int fileSet = 0; //Checks if inFile or outFile have been overwritten

	int numStrings = countStrings(splitString);
	
	//if first entry is input/output symbol, return error
	if(!strcmp(splitString[0],"<") || !strcmp(splitString[0],">"))
		return -1;

	optArgs[0] = splitString[0]; //set first arg to the command

	//save opt args into an array
	for(i=1;i<numStrings;++i) //start at 1 to skip initial command
	{
		//if prev entry was "<" or ">", skip over next word
		if(fileSet)
		{
			fileSet = 0;
			continue;
		}

		//if last entry is "&"
		if(i == numStrings-1 && !strcmp(splitString[i],"&"))
		{
			isBP = 1; //isBackgroundProcess is true
		}
		else if(!strcmp(splitString[i],">"))
		{
			//if ">" is last entry, return error
			if(i == numStrings-1)
				return -1;
			inFile = splitString[i-1]; //element before ">"
			outFile = splitString[i+1]; //element after ">"
			fileSet = 1; //Makes sure next element isn't saved as an opt arg
		}
		else if(!strcmp(splitString[i],"<"))
		{
			//if "<" is last entry, return error
			if(i == numStrings-1)
				return -1;
			//if stdout redirects to the command
			if(i == 1)
			{
				outFile = splitString[0]; //command
				inFile = splitString[i+1]; //element after "<"
				fileSet = 1; //Makes sure element after "<" isn't saved
			}
			else
			{
				outFile = splitString[i-1]; //element before "<"
				inFile = splitString[i+1]; //element after "<"
				fileSet = 1; //Makes sure element after "<" isn't saved
			}
		}
		//Add arg to array
		else
		{
			//if i is not the last element
			if(i != numStrings-1)
			{
				//if the next element is "<" or ">"
				if(!strcmp(splitString[i+1],"<") || !strcmp(splitString[i+1],">"))
				continue;
			}
			optArgs[optArgsIndex] = splitString[i];
			optArgsIndex++;
		}
	}

	if(inFile != '\0' && strcmp(splitString[0], inFile))
	{
		int checkExists = open(inFile, O_RDONLY);
		if(checkExists==-1)
		{
			(*status)=1;
			return -1;
		}
	}	

	beginChildProcess(splitString, optArgs, status, inFile, outFile, isBP, childPIDArray);
	return 1;
}

void shellDriver()
{
	int status = 0;
	int badExit = 0;
	char userInput[2048];
	pid_t childPIDArray[512];

	memset(userInput, '\0', sizeof(userInput));
	memset(childPIDArray, '\0', sizeof(childPIDArray));

	//Signal logic
	struct sigaction SIGINT_action = {{0}}, SIGTERM_action = {{0}}, ignore_action = {{0}};

	SIGINT_action.sa_handler = catchSIGINT;
	sigfillset(&SIGINT_action.sa_mask);
	SIGINT_action.sa_flags = 0;

	SIGTERM_action.sa_handler = catchSIGTERM;
	sigfillset(&SIGTERM_action.sa_mask);
	SIGINT_action.sa_flags = 0;

	ignore_action.sa_handler = SIG_IGN;

	sigaction(SIGINT, &SIGINT_action, NULL);
	sigaction(SIGHUP, &ignore_action, NULL); //ignore SIGHUP and SIGQUIT
	sigaction(SIGQUIT, &ignore_action, NULL);

	//Shell output
	while(1)
	{
		promptUser(userInput, childPIDArray, status);

		//check if command entered is built in
		//If so, performs the command, else returns 0
		if(checkBuiltInCommand(userInput, status, badExit) == 0)
		{
			badExit = 0; //reset

			//split user entry into string array
			char** splitString = splitSpaces(userInput);
		
			//organize command line into string array, start child process
			if(organizeCL(splitString, &status, childPIDArray) == -1)
			{
				paf("Error, invalid input\n");
				status = 1;
				badExit = 1;
				continue;
			}
		}
	}
}

int main()
{
	shellDriver();
	return 4;
}
