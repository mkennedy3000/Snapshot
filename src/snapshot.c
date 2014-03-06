/********************************
* Snapshot.c					*
*********************************
* Mark Kennedy: kenned31		*
* Jordan Ebel : ebel1			*
*********************************
* Description					*
*								*
* ..............				*
********************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "networking.h"
#include "file_io.h"

#define PORT 15457
#define YES 1
#define NO 0

//--- Message Format --------------------------------------------------------------------------------------------------//
//
// from:lamport:#widgets_given:#money_given:vector_timestamp<0>:vector_timestamp<1>:...:vector_timestamp<num_processes-1>
//
// Message is a marker if from is 125 or '}'
//--------------------------------------------------------------------------------------------------------------------//

//--- Delays ---//
#define MARKER_DELAY 5  //Marker Send Rate
#define MESSAGE_DELAY 2 //Message Send Rate
#define CHANNEL_DELAY 1 //Message Processing Rate
#define VERBOSE 1

//Global Variables for each process
int id;
int listenfd;
int num_processes;
int num_snapshots;
int total_snapshots;
FILE * snapshot_file;
int seed;

int last_marker = -1;

pthread_mutex_t money_mutex = PTHREAD_MUTEX_INITIALIZER;
int money = 100;
pthread_mutex_t widget_mutex = PTHREAD_MUTEX_INITIALIZER;
int widgets = 100;

pthread_mutex_t lamport_mutex = PTHREAD_MUTEX_INITIALIZER;
int lamport = 0;
pthread_mutex_t vector_mutex = PTHREAD_MUTEX_INITIALIZER;
int *vector;

//Talk Lock
pthread_mutex_t talk_mutex = PTHREAD_MUTEX_INITIALIZER;

//Virtual Message Channels
pthread_mutex_t channel_mutex = PTHREAD_MUTEX_INITIALIZER;
char **channel;
int *c_recorded;

//Channels that still need to be recorded
int record_c[5];

void lock(){
	pthread_mutex_lock(&money_mutex);
	pthread_mutex_lock(&widget_mutex);
	pthread_mutex_lock(&lamport_mutex);
	pthread_mutex_lock(&vector_mutex);
}

void unlock(){
	pthread_mutex_unlock(&money_mutex);
	pthread_mutex_unlock(&widget_mutex);
	pthread_mutex_unlock(&lamport_mutex);
	pthread_mutex_unlock(&vector_mutex);
}

void print_status(int mode, int w, int m, int to_from){
    if(VERBOSE){
        int i;
        printf("%d> L:%d <", id, lamport);
        for( i=0; i<num_processes; i++){
            printf("%d", vector[i]);
            if( i < num_processes-1 ) printf(",");
        }
        if(mode==0) printf("> Received: %d widgets and $%d from %d | ", w, m, to_from);
        if(mode==1) printf("> Sent: %d widgets and $%d to %d | ", w, m, to_from);
        printf("Widgets:%d Money:$%d\n", widgets, money);
    }
}

//Records Snapshots
void record_snapshot (int num, int respond) {
    record_state(snapshot_file, id, num, lamport, vector, money, widgets, channel, c_recorded, num_processes, record_c);
	
	//Send out markers to other processes
	int i;
	char marker[max_buf_len];
	marker[0] = '}';		//Marker
	marker[1] = id + 1; 	//From
	marker[2] = num + 1;	//Snapshot being recorded

	if (respond == YES){
	pthread_mutex_lock(&talk_mutex);
	struct addrinfo *p;
	for( i=0; i<num_processes; i++)
	{
		if ( i == id ) continue;
		int talkfd = set_up_talk(PORT+i, &p);
		if (talkfd != -1){
			int num_bytes = 0;
			while (num_bytes <= 0){
   	 			num_bytes = udp_send(talkfd, marker, p);
			}
			close(talkfd);
		}
	}
	pthread_mutex_unlock(&talk_mutex);
	}
}

//Take num_snapshots Snapshots (send out markers from process 0)
void *take_snapshots ()
{
	int i,j;

	int snaps = num_snapshots;
	for( i=0; i<snaps; i++)
	{
		//Take Snapshot Every 5 seconds		
		sleep(MARKER_DELAY);

		// Reset markers received
		for( j=0; j<num_processes; j++){
			record_c[j] = 1;
		}

		// Record own state and send out markers
		if(VERBOSE) printf("%d> Taking Snapshot %d\n", id, i);
		lock();
		record_snapshot(i,YES);
		unlock();
	}

	return 0;
}

//Process Received Messages
void *process_message (void *ptr)
{
	char *buf = (char *)ptr;
	sleep(CHANNEL_DELAY);

    int from = *((int *)buf) - 1;
    int l    = *((int *)buf+1) - 1;
    int w    = *((int *)buf+2) - 1;
    int m    = *((int *)buf+3) - 1;

	int i;
	int v[num_processes];
	for( i=4; i<num_processes+4; i++){
        v[i-4] = *((int *)buf+i)-1;
	}
		
	/*Update invariants*/
    lock();
	
    widgets += w;
	money += m;

	if (l < lamport) //lamport update
		lamport++;
	else
		lamport = l + 1;

	for( i=0; i<num_processes; i++){ //vector timestamp update
		if(i == id)
			vector[i]++;
		else if(vector[i] < v[i])
			vector[i] = v[i];
	}

    print_status(0, w, m, from);
	unlock();

	//Message Processed, Clear Channel
	pthread_mutex_lock(&channel_mutex);
	channel[from][0] = '\0'; //first byte to null signifies nothing in channel
	c_recorded[from] = YES;
	pthread_mutex_unlock(&channel_mutex);

	return 0;
}

//Run by read threads
void *read_messages ()
{
	//Continue receiving messages until all snapshots have been recorded
	char * buf = (char *)malloc(max_buf_len * sizeof(char));
	while(num_snapshots > 0){
		int num_bytes = udp_listen(listenfd, buf);
		if (num_bytes > 0){	
			//Check if marker
			if (buf[0] == '}'){
				int from = buf[1] - 1;
				int num = buf[2] -1;
				if( id != 0 ) {  //Process 0 already took its snapshot
					if (from == 0 /*&& last_marker != num*/){
						if(VERBOSE) printf("%d> Taking Snapshot %d\n", id, num);
						last_marker = num;
						int i;
						for (i=0; i<num_processes; i++){
							record_c[i] = 1;
						}
						record_snapshot(num,YES);
						num_snapshots--;
					}
					else /*if(from != 0)*/{ //Keep recording channels
						record_snapshot(num,NO);
						record_c[from] = 0;					
					}
				}
				else { //Process 0 updates
					record_c[from] = 0;

					//If all processes took a snapshot
					int snapshot_complete = 1;
					int i;
					for (i=0; i<num_processes; i++){
						if(record_c[i] == 1)
							snapshot_complete = 0;
					}
					if (snapshot_complete == 1)
						num_snapshots--;				
				}
			}
			else{
				//Put message into channel to process
				int from = buf[0] -1;
				pthread_mutex_lock(&channel_mutex);
				memcpy(channel[from], buf, max_buf_len);
				c_recorded[from] = NO;
				pthread_mutex_unlock(&channel_mutex);
				
				//Create a thread to process message when ready
				pthread_t process_thread;
				if (pthread_create(&process_thread, NULL, &process_message, (void *)channel[from])){
					printf("%d> Process Thread error\n", id);
				}
			}
		}
	}
	//free(buf);
	return 0;
}

//Run by write threads
void *write_messages ()
{
	while (num_snapshots > 0){	
        lock();

		int w = widgets==0 ? 0 : rand() % widgets + 1;
		int m = money==0 ? 0: rand() % money + 1;
		int sendto = rand() % num_processes;

        char message[max_buf_len];
		*((int *)message)   = id+1; //can't send 0 it is NULL
		*((int *)message+1) = lamport+1;
		*((int *)message+2) = w+1;
		*((int *)message+3) = m+1;
        
        int i;
		for( i=4; i<num_processes+4; i++){
			if( i-4 == id )
				vector[i-4]++;
            *((int *)message+i) = vector[i-4]+1;
		}

		widgets -= w;
		money -= m;
		lamport++;

		pthread_mutex_lock(&talk_mutex);
		struct addrinfo *p;
  		int talkfd = set_up_talk(PORT+sendto, &p);
		if(talkfd != -1){
   	    	udp_send(talkfd, message, p);
    	}
    	else{
     		printf("bad talkfd\n");
    	}
		close(talkfd);
		pthread_mutex_unlock(&talk_mutex);

        print_status(1, w, m, sendto);
        unlock();

		//Wait until another message can be sent
		sleep(MESSAGE_DELAY);
	}
	
	return 0;
}

//What every process runs
void run ()
{
	// Create Read/Write Threads
	pthread_t read_thread, write_thread;

	if (pthread_create(&read_thread, NULL, &read_messages, NULL)){
		printf("%d> Read Thread error\n", id);
	}
	if (pthread_create(&write_thread, NULL, &write_messages, NULL)){
		printf("%d> Write Thread error\n", id);
	}

	// Make Snapshot Marker Thread for Process 0
	if (id == 0)
	{
		pthread_t marker_thread;
		if (pthread_create(&marker_thread, NULL, &take_snapshots, NULL)){
			printf("%d> Marker Thread error\n", id);
		}
		pthread_join(marker_thread, NULL);
	}

	pthread_join(read_thread, NULL);
	pthread_join(write_thread, NULL);
}

int main (int argc, const char* argv[])
{
	int i = 0;

	if (argc != 4){
        printf("snapshot usage: num_processes num_snapshots random_seed\n");
        return -1;
	}
	else{
		num_processes = atoi(argv[1]);
		num_snapshots = atoi(argv[2]);
        total_snapshots = num_snapshots;
        seed = atoi(argv[3]);
	}

    if(VERBOSE) printf("processes: %d snapshots: %d \n", num_processes, num_snapshots);

	//Setup listening sockets and snapshot file descriptors
	int listenfds[num_processes];
    FILE * filefds[num_processes];
	for( i=0; i<num_processes; i++){
		listenfds[i] = set_up_listen(PORT+i);
        filefds[i] = open_file(i);
	}

	//Allocate and Initialize Vector Timestamps to <0,0,...>
	vector = (int *)malloc(num_processes * sizeof(int));
	for( i=0; i<num_processes; i++){
		vector[i] = 0;
	}

	//Allocate Message Channel Processing Queues
		//Messages are processed faster than they are sent, so only need
		//1 message receiving channel per process
	channel = (char **)malloc(num_processes * sizeof(char *));
	c_recorded = (int *)malloc(num_processes * sizeof(int));
	for( i=0; i<num_processes; i++ ){
		channel[i] = (char *)malloc(max_buf_len * sizeof(char));
		channel[i][0] = '\0';
		c_recorded[i] = NO;
	}

	//Create Processes and Run Program
	for( i=1; i<num_processes; i++ ){
		if( fork() == 0 ){
			id = i;
            srand(seed+i);
			listenfd = listenfds[i];
            snapshot_file = filefds[i];
			run();
			_exit(EXIT_SUCCESS);
		}
	}
	
	//Process 0
	id = 0;
    srand(seed);
	listenfd = listenfds[0];
    snapshot_file = filefds[0];
	run();

	//Close listening sockets and snapshot files
	for( i=0; i<num_processes; i++){
		close(listenfds[i]);
        close_file(filefds[i]);
	}
    
    return 0;
} 


