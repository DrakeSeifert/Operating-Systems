#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define NUM_THREADS 5

void* perform_work(void* argument) {
	int passed_in_value;
	
	passed_in_value = *((int *) argument);
	printf("Hello World! It's me, thread with argument %d\n", passed_in_value);
	return NULL;
}

int main() {

	pthread_t threads[5];
	int thread_args[5];
	int result_code, index;

	for(index=0; index < 5; ++index) {
		thread_args[index] = index;
		printf("In main: creating thread %d\n", index);
		result_code = pthread_create(&threads[index], NULL,
				perform_work, (void *)&thread_args[index]);
		assert(0 == result_code);
	}

	//... wait for each thread to complete
	
	for(index=0; index < 5; ++index) {
		result_code = pthread_join(threads[index], NULL);
		printf("In main: thread %d has completed\n", index);
		assert(0 == result_code);
	}

	printf("In main: All threads completed successfully\n");
	exit(0);
	
	/*
	pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_lock(myMutex);
	int resultCode = pthread_mutex_trylock(myMutex);
	pthread_mutex_unlock(myMutex);
	pthread_mutex_destroy(myMutex);
	*/
}
