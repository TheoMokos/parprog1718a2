#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>

#define N 100000    // number of elements for quicksort
#define NUMOFTHREADS 4   //number of threads in pool
#define CUTOFF 10     // cutoff to begin sorting with insertion sort
  

typedef struct{
    double *a;
    int n;
} message;

message global_queue[N];
int packets_created =0; //number of job packets created
int packets_completed =0; //numbers of job packets completed
int numbers_sorted =0;


pthread_cond_t packet_in = PTHREAD_COND_INITIALIZER;  // signal means new job got put in buffer - wait means we are waiting for more  jobs
pthread_cond_t packet_out = PTHREAD_COND_INITIALIZER; // signal means a job task got finished by a thread - wait means main thread waits for signals and every time checks if sorting finished
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void inssort(double *a,int n) {       //INSSORT FUNCTION
  int i,j;
  double t;
  
  for (i=1;i<n;i++) {
    j = i;
    while ((j>0) && (a[j-1]>a[j])) {
      t = a[j-1];  a[j-1] = a[j];  a[j] = t;
      j--;
    }
  }

}


int quicksort(double *a,int n) {      //QUICKSORT FUNCTION
  int first,last,middle;
  double t,p;
  int i,j;

  
  // take first, last and middle positions
  first = 0;
  middle = n-1;
  last = n/2;  
  
  // put median-of-3 in the middle
  if (a[middle]<a[first]) { t = a[middle]; a[middle] = a[first]; a[first] = t; }
  if (a[last]<a[middle]) { t = a[last]; a[last] = a[middle]; a[middle] = t; }
  if (a[middle]<a[first]) { t = a[middle]; a[middle] = a[first]; a[first] = t; }
    
  // partition (first and last are already in correct half)
  p = a[middle]; // pivot
  for (i=1,j=n-2;;i++,j--) {
    while (a[i]<p) i++;
    while (p<a[j]) j--;
    if (i>=j) break;

    t = a[i]; a[i] = a[j]; a[j] = t;      
  }
  return i;
  
}



void *work()       //WORK FUNCTIONS
{
	
	message *aa; 

	while(true){
		pthread_mutex_lock(&mutex);
		while (packets_completed < packets_created){
			printf("Reading packet...");
			pthread_cond_wait(&packet_in, &mutex); 
		}
		if (exitcond){
      break;
    } 
	  printf("Reading job...\n");
    
    aa = &global_queue[packets_completed];
    packets_completed++;
		pthread_mutex_unlock(&mutex);


		
		if ( (*aa).n <= CUTOFF){
	  	inssort((*aa).a,(*aa).n);
		}
		else{

			int job = quicksort((*aa).a, (*aa).n);
			pthread_mutex_lock(&mutex);
			addJob((*temp).a, i);  // a -> i
      global_queue[packets_completed].a = aa;
      global_queue[packets_completed].n = job;
			
			pthread_mutex_unlock(&mutex);
			pthread_cond_signal(&packet_in); 
		}
		pthread_cond_signal(&packet_out);
	}
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}




int main()
{
    
  double *a = (double *)malloc(N * sizeof(double));
	srand(time(NULL));
	for (int i = 0; i < N; i++){
		a[i] = rand()/ RAND_MAX;
	}


   //creating the threads
	pthread_t * pth_array = malloc(NUMOFTHREADS * sizeof(pthread_t));
    for(int i = 0; i < NUMOFTHREADS; i++)
        pthread_create(&pth_array[i], NULL, work, (void*)((long)i));


	// first job packet
	
  global_queue[packets_created].a = a;
  global_queue[packets_created].n = n;
  packets_created++;
	pthread_cond_signal(&packet_in); // signal threads that job is in


	
	printf("\n Getting ready for shutdown \n");
	pthread_mutex_lock(&mutex);
	while(numbers_sorted != N){
		pthread_cond_wait(&packet_out, &mutex);
		
	}
	
  printf("Exit.......\n");
	exit_condition = 1;
	
  pthread_cond_broadcast(&packet_in); // tell every thread to wake, and then cond=1 so they break the while loop and exit
	
  pthread_mutex_unlock(&mutex);


	// checking sorting
  for (int i = 0; i < (N - 1); i++){
		if (a[i] > a[i + 1]){
			printf("Quicksort failure.... \n");
			break;
		}else{
			printf("Sorting Complete! Good Job!\n");
		}
	}
	


  //thread join
  for (int i = 0; i < NUMOFTHREADS; i++){
        pthread_join(pth_array[i], NULL);
		
	}

	//free allocations
	free(pth_array);
	free(a);
	pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&packet_in);
	pthread_cond_destroy(&packet_out);
	


	// EXIT
	return 0;
}
