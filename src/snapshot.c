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
#include "networking.h"

#define PORT 15457

//Global Variables for each process
int id;
int listenfd;
int num_processes;
int num_snapshots;

pthread_mutex_t money_mutex = PTHREAD_MUTEX_INITIALIZER;
int money = 100;
pthread_mutex_t widget_mutex = PTHREAD_MUTEX_INITIALIZER;
int widgets = 100;

//Run by read threads
void *read_messages ()
{
	printf("%d> In Read Thread\n", id);
	return 0;
}

//Run by write threads
void *write_messages ()
{
	printf("%d> In Write Thread\n", id);
	return 0;
}

void run ()
{
	/*Test Code*/
	//int i;
	char message[15];
	sprintf(message, "Hello from %d", id);

	//Set Up Talking Socket Each Time
	struct addrinfo *p;
   	int talkfd = set_up_talk(PORT+id, &p);

    if(talkfd != -1){
        int num_bytes = udp_send(talkfd, message, p);
        printf("%d> Sent %d bytes\n", id, num_bytes);
    }
    else{
        printf("bad talkfd\n");
    }
	
    char * buf = (char *)malloc(100 * sizeof(char));
    if(listenfd != -1){
		//for(i=0; i<num_processes; i++){
            int num_bytes = udp_listen(listenfd, buf);    
            printf("%d> Received: %d bytes\n", id, num_bytes);
            printf("%d> Received: %s\n", id, buf);
		//}
    }
    else{
        printf("%d> Bad listenfd\n", id);
    }
	
    printf("%d> Done\n", id);
	/*End Test Code*/

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
	for( i=num_processes-1; i>0; i-- ){
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
        
    return 0;
} 


