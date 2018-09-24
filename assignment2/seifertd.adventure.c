#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>

#define NUMROOMFILES 7
#define TESTING 0

struct myRooms {
	char* name;
	char* roomType;
};

char** assignRooms(char** roomNames) {
	char** chosenRooms= (char**)malloc(NUMROOMFILES*sizeof(char*));
	int availableNum[10] = {1,1,1,1,1,1,1,1,1,1};
	int myRandNum;
	int i;

	//Assign random rooms to chosenRooms
	for(i=0; i<NUMROOMFILES; ++i) {
		myRandNum = rand()%10;
		//Find number 0-9 that hasn't been
		//previously chosen:
		while(!availableNum[myRandNum]) {
			myRandNum = rand()%10;
		}
		chosenRooms[i] = roomNames[myRandNum];
		availableNum[myRandNum] = 0;
	}
	return chosenRooms;
}

struct myRooms* initializeMyRoom(char* name, char* roomType) {
	struct myRooms* room = malloc(sizeof(struct myRooms));
	room->name = name;
	room->roomType = roomType;
	return room;
}

int IsGraphFull(int connectRooms[NUMROOMFILES][NUMROOMFILES]) {
	int i,j;
	int count = 0;
	int success = 0;

	//Check each room for at least 3 connections
	for(i=0;i<NUMROOMFILES;++i) {
		for(j=0;j<NUMROOMFILES;++j) {
			if(connectRooms[i][j] == 1)
				count++;
		}
		if(count >= 3) {
			success++;
		}
		count = 0;
	}
	
	//If all 7 rooms have at least 3 connections, return 1
	if(success == NUMROOMFILES)
		return 1;
	else
		return 0;
}

//Check for valid room connection
int CanAddConnectionFrom(int myRoom, int myArray[NUMROOMFILES][NUMROOMFILES]) {
	int i;
	for(i=0;i<NUMROOMFILES;++i){
		if(myArray[myRoom][i] == 0 && myRoom!=i)
			return 1;
	}
	return 0;
}

int IsSameRoom(int RoomA, int RoomB) {
	if(RoomA == RoomB)
		return 1;
	else
		return 0;
}

void ConnectRoom(int RoomA, int RoomB, int myArray[][NUMROOMFILES]) {
	int i;
	int j;
	myArray[RoomA][RoomB] = 1;
	return;
}

int inEndRoom(char currentFile[100]) {
	FILE* fp;
	char buf[1000];
	memset(buf, '\0', sizeof(buf));
	fp = fopen(currentFile, "r");

	while(fgets(buf, sizeof(buf), fp)) {
		if(strstr(buf,"END_ROOM")) {
			return 1;
		}
	}

	//Delete trailing newline created by fgets
	currentFile[strcspn(currentFile, "\n")] = 0;
	fclose(fp);
	return 0;
}

char* getLoca(char currentFile[100]) {

	int fd;
	ssize_t nread;
	char* daLoca;
	daLoca = (char *)malloc(6*sizeof(char));
	memset(daLoca, '\0', sizeof(daLoca));
	fd = open(currentFile, O_RDONLY);
	lseek(fd, 0, SEEK_SET);
	if(fd < 0) {
		fprintf(stderr, "Could not open %s\n", 
					   currentFile);
		exit(1);
	}
	lseek(fd, 11, SEEK_SET); //Set file pointer to room name
	nread = read(fd, daLoca, 5); //RoomX has 5 chars
	daLoca[6] = '\0'; //Add null terminator
	close(fd);	

	return daLoca;
}

int countLines(char currentFile[100]) {
	FILE *fp;
	fp = fopen(currentFile, "r");
	int lineCount=0;
	char ch;
	while((ch = fgetc(fp)) != EOF) {
		if(ch == '\n')
			lineCount++;
	}
	fclose(fp);

	return lineCount;	
}

char* getConnect(char currentFile[100], int roomConnect) {
	int fd;
	ssize_t nread;
	char* myConnect;
	myConnect = (char *)malloc(6*sizeof(char));
	memset(myConnect, '\0', sizeof(myConnect));
	fd = open(currentFile, O_RDONLY);
	lseek(fd, 0, SEEK_SET);
	if(fd < 0) {
		fprintf(stderr, "Could not open %s\n", 
					   currentFile);
		exit(1);
	}
	int startByte = 31 + 20*roomConnect;
	lseek(fd, startByte, SEEK_SET); //Set file pointer to room name
	nread = read(fd, myConnect, 5); //RoomX has 5 chars
	myConnect[6] = '\0'; //Add null terminator
	close(fd);

	return myConnect;
}

int validInput(char currentFile[100], char userInput[100]) {
	
	FILE* fp;
	char buf[1000];
	memset(buf, '\0', sizeof(buf));
	fp = fopen(currentFile, "r");
	fseek(fp, 31, SEEK_SET); 

	while(fgets(buf, sizeof(buf), fp)) {
		if(strstr(buf, userInput)) {
			return 1;
		}
	}

	//Delete trailing newline created by fgets
	currentFile[strcspn(currentFile, "\n")] = 0;
	fclose(fp);
	return 0;
}

void getTime() {

	time_t t2 = time(NULL);
	struct tm tm = *localtime(&t2);

	const char* month[12] = {"January", "February", "March", "April", "May", "June", 
			"July", "August", "September", "October", "November", "December"};

	const char* weekday[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
				 "Friday", "Saturday"};

	int hour = tm.tm_hour;
	if(hour>12)
		hour-=12;

	//Write to file
	FILE *fp;
	fp = fopen("./currentTime.txt", "wb");
	fprintf(fp, "%d:%d, %s, %s %d, %d\n",hour,tm.tm_min,weekday[tm.tm_wday],
					month[tm.tm_mon],tm.tm_mday,tm.tm_year+1900);
	fclose(fp);


	return;
}

void printTime() {

	//Read from file and print contents
	FILE *fp;
	int c;
	fp = fopen("./currentTime.txt","r");
	printf("\n");
	c = fgetc(fp);
	while (c!=EOF) {
		printf("%c",c);
		c = fgetc(fp);
	}
	return;
}

//pthread_mutex_t lock;


//void* perform_work(void* argument) {
//
//	pthread_mutex_lock(&lock);
//	printf("IN TOP perform_work\n");
	/*
	int passed_in_value;
	passed_in_value = *((int *) argument);
	printf("Hello World! It's me, thread with argument %d\n", passed_in_value);
	return NULL;
	*/

	//int passed_in_value = *((int *) argument);
//	getTime();
//	
//	pthread_mutex_unlock(&lock);
//	printf("IN BOTTOM perform_work\n");
//	return NULL;
//}

int main() {
	time_t t;
	srand((unsigned) time(&t));
	
	//Rooms to choose from
	char* roomNames[10] = {"Room0","Room1","Room2","Room3","Room4",
			     "Room5","Room6","Room7","Room8","Room9"};

	//Randomly pick 7 rooms
	char** chosenRooms = assignRooms(roomNames);

	//TEST: 7 rooms randomly chosen
	int i;
	if(TESTING) {
		printf("\nTEST random rooms:\n");

		for(i=0; i<NUMROOMFILES; ++i) {
			printf("%s\n",chosenRooms[i]);
		}
	}

	//Create room objects, initialize name and room type	
	struct myRooms* room0 = initializeMyRoom(chosenRooms[0],
						"START_ROOM");
	struct myRooms* room1 = initializeMyRoom(chosenRooms[1],
						"MID_ROOM");
	struct myRooms* room2 = initializeMyRoom(chosenRooms[2],
						"MID_ROOM");
	struct myRooms* room3 = initializeMyRoom(chosenRooms[3],
						"MID_ROOM");
	struct myRooms* room4 = initializeMyRoom(chosenRooms[4],
						"MID_ROOM");
	struct myRooms* room5 = initializeMyRoom(chosenRooms[5],
						"MID_ROOM");
	struct myRooms* room6 = initializeMyRoom(chosenRooms[6],
						"END_ROOM");

	//TEST: Check room types are correct
	if(TESTING) {
		printf("\nTEST room types:\n");
		printf("room0 type: %s\n",room0->roomType);
		printf("room1 type: %s\n",room1->roomType);
		printf("room6 type: %s\n\n",room6->roomType);
	}

	//Connect Rooms
	
	//initialize all room connections as false
	int connectRooms[NUMROOMFILES][NUMROOMFILES];
	int j;
	for (i=0;i<NUMROOMFILES;++i){
		for(j=0;j<NUMROOMFILES;++j){
			connectRooms[i][j] = 0;
		}
	}

	//TEST: check array contents are all zero
	if(TESTING) {
		printf("TEST connectRooms array initialized\n");
		int testCount = 0;
		for (i=0;i<NUMROOMFILES;++i){
			for(j=0;j<NUMROOMFILES;j++){
				printf("%d ",connectRooms[i][j]);
				testCount++;
				if(testCount == NUMROOMFILES) {
					printf("\n");
					testCount = 0;
				}
			}
		}
	}

	while(!IsGraphFull(connectRooms)) {

		int RoomA = -1; //Rows
		int RoomB; //Cols

		while(1) {
			RoomA = rand()%7;
			if(CanAddConnectionFrom(RoomA, connectRooms) == 1)
				break;
		}

		do {
			RoomB = rand()%7;
		} while(!(CanAddConnectionFrom(RoomB, connectRooms) == 1)
			  || IsSameRoom(RoomA, RoomB));

		ConnectRoom(RoomA, RoomB, connectRooms);
		ConnectRoom(RoomB, RoomA, connectRooms);
	}

	//TEST: check array contents are all zero
	if(TESTING) {
		printf("\nTEST connectRooms array filled\n");
		int testCount = 0;
		for (i=0;i<NUMROOMFILES;++i){
			for(j=0;j<NUMROOMFILES;j++){
				printf("%d ",connectRooms[i][j]);
				testCount++;
				if(testCount == NUMROOMFILES) {
					printf("\n");
					testCount = 0;
				}
			}
		}
	}

	char* gameRooms[7] = {room0->name,room1->name,room2->name,
			      room3->name,room4->name,room5->name,
			      room6->name};

	//Create directory for room files
	char myNewDir[100];
	memset(myNewDir,'\0',100);
	sprintf(myNewDir,"./seifertd.rooms.%d",getpid());
	mkdir(myNewDir, 0700);

	//Append file name to myNewDir
	char finalPath0[100];
	memset(finalPath0,'\0',100);
	sprintf(finalPath0,"%s/%s", myNewDir,room0->name);

	char finalPath1[100];
	memset(finalPath1,'\0',100);
	sprintf(finalPath1,"%s/%s", myNewDir,room1->name);

	char finalPath2[100];
	memset(finalPath2,'\0',100);
	sprintf(finalPath2,"%s/%s", myNewDir,room2->name);

	char finalPath3[100];
	memset(finalPath3,'\0',100);
	sprintf(finalPath3,"%s/%s", myNewDir,room3->name);

	char finalPath4[100];
	memset(finalPath4,'\0',100);
	sprintf(finalPath4,"%s/%s", myNewDir,room4->name);

	char finalPath5[100];
	memset(finalPath5,'\0',100);
	sprintf(finalPath5,"%s/%s", myNewDir,room5->name);

	char finalPath6[100];
	memset(finalPath6,'\0',100);
	sprintf(finalPath6,"%s/%s", myNewDir,room6->name);

	//Write each room to its own file
	FILE* fp0;
	fp0 = fopen(finalPath0, "a");
	fprintf(fp0, "ROOM NAME: %s\n", room0->name);
	int conNum = 1;
	for(j=0;j<NUMROOMFILES;++j){
		if(connectRooms[0][j]){
			fprintf(fp0, "CONNECTION %d: %s\n",
				conNum,gameRooms[j]);
				conNum++;
		}
	}
	fprintf(fp0, "ROOM TYPE: %s\n", room0->roomType);
	fclose(fp0);

	FILE* fp1;
	fp1 = fopen(finalPath1, "a");
	fprintf(fp1, "ROOM NAME: %s\n", room1->name);
	conNum = 1;
	for(j=0;j<NUMROOMFILES;++j){
		if(connectRooms[1][j]){
			fprintf(fp1, "CONNECTION %d: %s\n",
				conNum,gameRooms[j]);
				conNum++;
		}
	}
	fprintf(fp1, "ROOM TYPE: %s\n", room1->roomType);
	fclose(fp1);

	FILE* fp2;
	fp2 = fopen(finalPath2, "a");
	fprintf(fp2, "ROOM NAME: %s\n", room2->name);
	conNum = 1;
	for(j=0;j<NUMROOMFILES;++j){
		if(connectRooms[2][j]){
			fprintf(fp2, "CONNECTION %d: %s\n",
				conNum,gameRooms[j]);
				conNum++;
		}
	}
	fprintf(fp2, "ROOM TYPE: %s\n", room2->roomType);
	fclose(fp2);

	FILE* fp3;
	fp3 = fopen(finalPath3, "a");
	fprintf(fp3, "ROOM NAME: %s\n", room3->name);
	conNum = 1;
	for(j=0;j<NUMROOMFILES;++j){
		if(connectRooms[3][j]){
			fprintf(fp3, "CONNECTION %d: %s\n",
				conNum,gameRooms[j]);
				conNum++;
		}
	}
	fprintf(fp3, "ROOM TYPE: %s\n", room3->roomType);
	fclose(fp3);

	FILE* fp4;
	fp4 = fopen(finalPath4, "a");
	fprintf(fp4, "ROOM NAME: %s\n", room4->name);
	conNum = 1;
	for(j=0;j<NUMROOMFILES;++j){
		if(connectRooms[4][j]){
			fprintf(fp4, "CONNECTION %d: %s\n",
				conNum,gameRooms[j]);
				conNum++;
		}
	}
	fprintf(fp4, "ROOM TYPE: %s\n", room4->roomType);
	fclose(fp4);

	FILE* fp5;
	fp5 = fopen(finalPath5, "a");
	fprintf(fp5, "ROOM NAME: %s\n", room5->name);
	conNum = 1;
	for(j=0;j<NUMROOMFILES;++j){
		if(connectRooms[5][j]){
			fprintf(fp5, "CONNECTION %d: %s\n",
				conNum,gameRooms[j]);
				conNum++;
		}
	}
	fprintf(fp5, "ROOM TYPE: %s\n", room5->roomType);
	fclose(fp5);

	FILE* fp6;
	fp6 = fopen(finalPath6, "a");
	fprintf(fp6, "ROOM NAME: %s\n", room6->name);
	conNum = 1;
	for(j=0;j<NUMROOMFILES;++j){
		if(connectRooms[6][j]){
			fprintf(fp6, "CONNECTION %d: %s\n",
				conNum,gameRooms[j]);
				conNum++;
		}
	}
	fprintf(fp6, "ROOM TYPE: %s\n", room6->roomType);
	fclose(fp6);

	if(TESTING) {
		int fd;
		ssize_t nread;
		char readBuffer[1000];
		memset(readBuffer, '\0', sizeof(readBuffer));
		fd = open(finalPath0, O_RDONLY);
		if(fd < 0) {
			fprintf(stderr, "Could not open %s\n", 
						   finalPath0);
			exit(1);
		}
		lseek(fd, 0, SEEK_SET);//reset filePointer to beginning
		nread = read(fd, readBuffer, sizeof(readBuffer));
	
		printf("\nTEST File contents:\n%s", readBuffer);
		printf("\n");
	}

	//initialize starting room
	char currentFile[1000];
	memset(currentFile, '\0', sizeof(currentFile));
	strcpy(currentFile, finalPath0);

	char* myLoca;                               //Location string
	int lineCount = 0;
	int roomConnect = 0;			    //Num connections
	char* myConnect;			    //Connection string
	char userInput[100];
	memset(userInput, '\0', sizeof(userInput));
	int numSteps = 0;
	FILE* temp; //temp file keeps track of user path
	char tempPath[100];
	memset(tempPath,'\0',100);
	sprintf(tempPath,"%s/%s", myNewDir,"tmp");

	//printf("TEST: tempPath is %s\n",tempPath);

	while(!inEndRoom(currentFile)) {

		printf("\n");

		//Location logic
		myLoca = getLoca(currentFile);
		printf("CURRENT LOCATION: %s\n", myLoca);

		//Connections logic
		lineCount = countLines(currentFile);
		printf("POSSIBLE CONNECTIONS: ");
		for(i=0;i<lineCount-2;++i) {
			myConnect=getConnect(currentFile, roomConnect);
			printf("%s", myConnect);
			roomConnect++;

			if(i == lineCount-3) { //last connection
				printf(".\n");
				roomConnect = 0;
			}
			else
				printf(", ");
		}

		//User input logic
		printf("WHERE TO? >");

		if(fgets(userInput, sizeof(userInput), stdin)) {
			if(!strcmp(userInput, "time\n")) {

				//Write time to file	
				getTime();

				/*
				if(pthread_mutex_init(&lock, NULL) != 0) {
					printf("\nmutex init failed\n");
					return 1;
				}
				pthread_t tid[0];
				int err = pthread_create(&(tid[0]),NULL,&perform_work,NULL);
				pthread_join(tid[0],NULL);
				
				pthread_mutex_destroy(&lock);
				*/

				printTime();

			}

			else if(validInput(currentFile, userInput)) {
				sprintf(currentFile,"%s/%s", myNewDir,userInput);

				//Delete trailing newline created by fgets
				currentFile[strcspn(currentFile, "\n")] = 0;

				//Write path to file
				temp = fopen(tempPath, "a");
				fprintf(temp, "%s", userInput);
				fclose(temp);

				numSteps++;
			}
			else {
				printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
			}
		}
	}
	printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n",numSteps);

	temp = fopen(tempPath, "r");
	int c;
	if(temp) {
		while((c = getc(temp)) != EOF)
			putchar(c);
		fclose(temp);
	}


	//Remove temp file
	remove(tempPath);

	//Deallocate memory
	free(room0);
	free(room1);
	free(room2);
	free(room3);
	free(room4);
	free(room5);
	free(room6);

	free(chosenRooms);

	free(myLoca);
	free(myConnect);

	return 0;
}
