// Condition variable usage example.
// Compile with: gcc -O2 -Wall -pthread quicksort.c -o quicksort

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MESSAGES 20
#define Q_SIZE 1000
#define THREADS 4
#define N 1000000
#define CUTOFF 10


// global integer buffer
int global_buffer;
// global avail messages count (0 or 1)
int global_availmsg = 0;	// empty
// condition variable, signals a put operation (receiver waits on this)
pthread_cond_t msg_in = PTHREAD_COND_INITIALIZER;
// condition variable, signals a get operation (sender waits on this)
pthread_cond_t msg_out = PTHREAD_COND_INITIALIZER;
// mutex protecting common resources
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int global_queue_in = 0;
int global_queue_out = 0;

struct message{
	double *a;
	int n;
};

struct message queue[Q_SIZE];

//Queue management
void produce(double *a, int n){
	
	queue[global_queue_in ].a=a;
	queue[global_queue_in ].n=n;
	
	global_queue_in +=1;
	if (global_queue_in >=Q_SIZE){
		global_queue_in =0;
	}
	
	global_availmsg+=1;
	
}

void consume(double **a, int *n){
	
	*a=queue[global_queue_out].a;
	*n=queue[global_queue_out].n;

	global_queue_out+=1;
	if (global_queue_out>=Q_SIZE){
		global_queue_out=0;
	}

	global_availmsg+=1;
	
}

// producer thread function
void *producer_thread(void *args) {
  int i;
  
  // send a predefined number of messages
  for (i=0;i<MESSAGES;i++) {
    // lock mutex
    pthread_mutex_lock(&mutex);
    while (global_availmsg>0) {	// NOTE: we use while instead of if! more than one thread may wake up
    				
      pthread_cond_wait(&msg_out,&mutex);  // wait until a msg is received - NOTE: mutex MUST be locked here.
      					   // If thread is going to wait, mutex is unlocked automatically.
      					   // When we wake up, mutex will be locked by us again. 
    }
    // send message
    //printf("Producer: sending msg %d\n",i);
    global_buffer = i;
    global_availmsg = 1;
    
    // signal the receiver that something was put in buffer
    pthread_cond_signal(&msg_in);
    
    // unlock mutex
    pthread_mutex_unlock(&mutex);
  }
  
  // exit and let be joined
  pthread_exit(NULL); 
}
  
  
// receiver thread function
void *consumer_thread(void *args) {
  int i;
  
  // receive a predefined number of messages
  for (i=0;i<MESSAGES;i++) {
    // lock mutex
    pthread_mutex_lock(&mutex);
    while (global_availmsg<1) {	// NOTE: we use while instead of if! more than one thread may wake up
      pthread_cond_wait(&msg_in,&mutex); 
    }
    // receive message
    //printf("Consumer: received msg %d\n",global_buffer);
    global_availmsg = 0;
    
    // signal the sender that something was removed from buffer
    pthread_cond_signal(&msg_out);
    
    // unlock mutex
    pthread_mutex_unlock(&mutex);
  }
  
  // exit and let be joined
  pthread_exit(NULL); 
}

void inssort(double *a, int n){

	int i,j;
	double t;
	
	for (i=1;i<n;i++){
		j=i;
		while ((j>0) && (a[j-1]>a[j])){
			t=a[j-1]; a[j-1]=a[j]; a[j]=t;
			j--;
		}
	}
}

int partition(double *a, int n){
	

	double t,p;
	int first=0;
	int middle=n/2;
	int last=n-1;
	int i,j;

	if(a[middle]<a[first]){
		t=a[middle]; a[middle]=a[first]; a[first]=t;
	}

	if(a[middle]>a[last]){
		t=a[middle]; a[middle]=a[last]; a[last]=t;
	}

	if(a[middle]<a[first]){
		t=a[middle]; a[middle]=a[first]; a[first]=t;
	}

	p=a[middle];
	for(i=1,j=n-2;;i++,j--){
		while (a[i]<p) i++;
		while (a[j]>p) j--;
		if (i>=j) break;
		
		t=a[i]; a[i]=a[j]; a[j]=t;
	}
	
	return i; 
}	

void quicksort(double *a,int n){

	int i;
	if (n<=CUTOFF){
		inssort(a,n);
		return;
	}
	
	i=partition(a,n);
	quicksort(a,i);
	quicksort(a+i,n-i);
}

/*void *thread_func(void *args){
	
	
	pthread_t worker1,worker2,worker3,worker4;


	
	
	
	pthread_exit(NULL); 
}*/


int main() {
  
  pthread_t producer,consumer;
  
  // create threads
  pthread_create(&producer,NULL,producer_thread,NULL);
  pthread_create(&consumer,NULL,consumer_thread,NULL);
  
  // then join threads
  pthread_join(producer,NULL);
  pthread_join(consumer,NULL);

  // destroy mutex - should be unlocked
  pthread_mutex_destroy(&mutex);

  // destroy cvs - no process should be waiting on these
  pthread_cond_destroy(&msg_out);
  pthread_cond_destroy(&msg_in);


	double *array;
	int i = 0;

	array = malloc(N * sizeof(double));
	if (i!=0) exit (1);

	srand(time(NULL));
	for (i=0;i<N;i++){
		array[i]=(double)rand()/RAND_MAX;
	}


  return 0;
}
