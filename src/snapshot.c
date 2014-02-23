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
#include "networking.h"

#define PORT 15457

void run ( int id, int listenfd, int num_processes, int num_snapshots )
{
	/*Test Code*/
	int i;
	char message[15];
	sprintf(message, "Hello from %d", id);

	//Set Up Talking Socket Each Time
	struct addrinfo *p;
   	int talkfd = set_up_talk(PORT+1, &p);

    if(talkfd != -1){
        int num_bytes = udp_send(talkfd, message, p);
        printf("%d> Sent %d bytes\n", id, num_bytes);
    }
    else{
        printf("bad talkfd\n");
    }
	
    char * buf = (char *)malloc(100 * sizeof(char));
    if(listenfd != -1){
		for(i=0; i<num_processes; i++){
            int num_bytes = udp_listen(listenfd, buf);    
            printf("%d> Received: %d bytes\n", id, num_bytes);
            printf("%d> Received: %s\n", id, buf);
		}
    }
    else{
        printf("%d> Bad listenfd\n", id);
    }
	
    printf("%d> Done\n", id);
	/*End Test Code*/

	//MP logic here
	//Do something until num_snapshots have been taken
}

int main (int argc, const char* argv[])
{

    int num_processes = 0;
    int num_snapshots = 0;
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
	int listenfd[num_processes];
	for( i=0; i<num_processes; i++){
		listenfd[i] = set_up_listen(PORT+i);
	}

	//Create Processes and Run Logic
	for( i=num_processes-1; i>0; i-- ){
		if( fork() == 0 ){
			run(i, listenfd[i], num_processes, num_snapshots);
			_exit(EXIT_SUCCESS);
		}
	}
	
	//Process 0
	run(0, listenfd[0], num_processes, num_snapshots);
        
    return 0;
} 


