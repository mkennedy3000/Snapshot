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

#define PORT 15457

//--- Message Format ---//
//
// lamport_from, lamport, #widgets_given, #money_given
//
//----------------------//

//Global Variables for each process
int id;
int listenfd;
int num_processes;
int num_snapshots;

pthread_mutex_t money_mutex = PTHREAD_MUTEX_INITIALIZER;
int money = 100;
pthread_mutex_t widget_mutex = PTHREAD_MUTEX_INITIALIZER;
int widgets = 100;

pthread_mutex_t lamport_mutex = PTHREAD_MUTEX_INITIALIZER;
int lamport = 0;

//Run by read threads
void *read_messages ()
{
	//TEST: Read forever
	char * buf = (char *)malloc(100 * sizeof(char));
	while(1){
		int num_bytes = udp_listen(listenfd, buf);

		if (num_bytes > 0){	//Change num_bytes with message size	
			int l = buf[0];
			int from = buf[1] -1;
			int w = buf[2] -1;
			int m = buf[3] -1;
		
			//Update invariants
			pthread_mutex_lock(&money_mutex);
			pthread_mutex_lock(&widget_mutex);
			pthread_mutex_lock(&lamport_mutex);

			widgets += w;
			money += m;

			if (l < lamport)
				lamport++;
			else
				lamport = l + 1;

			printf("%d> L:%d Received: %d widgets and $%d from %d\n", id, lamport, w, m, from);
			printf("%d> Widgets:%d Money:$%d\n", id, widgets, money);

			pthread_mutex_unlock(&money_mutex);
			pthread_mutex_unlock(&widget_mutex);
			pthread_mutex_unlock(&lamport_mutex);
		}
	}
	return 0;
}

//Run by write threads
void *write_messages ()
{
	//TEST: Send 10 Widgets and $20 to Process 1 from *
	if (id != 1){	
		pthread_mutex_lock(&money_mutex);
		pthread_mutex_lock(&widget_mutex);
		pthread_mutex_lock(&lamport_mutex);

		int w = 10;
		int m = 20;

		char message[4];
		message[0] = lamport+1;
		message[1] = id+1; //can't send 0 it is NULL
		message[2] = w+1;
		message[3] = m+1;

		widgets -= w;
		money -= m;
		lamport++;

		struct addrinfo *p;
   		int talkfd = set_up_talk(PORT+1, &p);
		if(talkfd != -1){
   	    	int num_bytes = udp_send(talkfd, message, p);
   	    	printf("%d> Sent %d bytes\n", id, num_bytes);
    	}
    	else{
     	   printf("bad talkfd\n");
    	}

		printf("%d> L:%d Sent: %d widgets and $%d to %d\n", id, lamport, w, m, 1);
		printf("%d> Widgets:%d Money:$%d\n", id, widgets, money);

		pthread_mutex_unlock(&money_mutex);
		pthread_mutex_unlock(&widget_mutex);
		pthread_mutex_unlock(&lamport_mutex);

	}

	return 0;
}

//What every process runs
void run ()
{
	//MP logic here
	pthread_t read_thread, write_thread;

	if (pthread_create(&read_thread, NULL, &read_messages, NULL)){
		printf("%d> Read Thread error\n", id);
	}
	if (pthread_create(&write_thread, NULL, &write_messages, NULL)){
		printf("%d> Write Thread error\n", id);
	}

	pthread_join(read_thread, NULL);
	pthread_join(write_thread, NULL);
}

int main (int argc, const char* argv[])
{
	int i = 0;

	if (argc != 3){
        printf("snapshot usage: num_processes num_snapshots\n");
        return -1;
	}
	else{
		num_processes = atoi(argv[1]);
		num_snapshots = atoi(argv[2]);
	}

    printf("processes: %d snapshots: %d \n", num_processes, num_snapshots);

	//Setup Listening Sockets
	int listenfds[num_processes];
	for( i=0; i<num_processes; i++){
		listenfds[i] = set_up_listen(PORT+i);
	}

	//Create Processes and Run Program
	for( i=1; i<num_processes; i++ ){
		if( fork() == 0 ){
			id = i;
			listenfd = listenfds[i];
			run();
			_exit(EXIT_SUCCESS);
		}
	}
	
	//Process 0
	id = 0;
	listenfd = listenfds[0];
	run();

	//Close Listening Sockets
	for( i=0; i<num_processes; i++){
		close(listenfds[i]);
	}
        
    return 0;
} 


