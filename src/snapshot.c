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

int main (int argc, const char* argv[])
{

    int num_processes = 0;
    int num_snapshots = 0;

	if (argc != 3){
        printf("snapshot usage: num_processes num_snapshots\n");
        return -1;
	}
	else{
		num_processes = atoi(argv[1]);
		num_snapshots = atoi(argv[2]);
	}

    printf("process: %d snapshots: %d \n", num_processes, num_snapshots);

    pid_t pid = fork();

    if(pid == -1){
        printf("fork() failure\n");
        return -1;
    }
    else if(pid == 0){   //child process
        struct addrinfo *p;
        int talkfd = set_up_talk("19457", &p);

        if(talkfd != -1){
            int num_bytes = udp_send(talkfd, "Hello", p);
            printf("Sent %d bytes\n", num_bytes);
            close(talkfd);
        }
        else{
            printf("bad talkfd\n");
        }
        
        printf("child done\n");
        fflush(stdout);
        _exit(EXIT_SUCCESS);
    }
    else{   //parent process
        int listenfd = set_up_listen("19457");
        char * buf = (char *)malloc(100 * sizeof(char));

        if(listenfd != -1){
            int num_bytes = udp_listen(listenfd, buf);    
            printf("Received: %d bytes\n", num_bytes);
            printf("Received: %s\n", buf);
            close(listenfd);
        }
        else{
            printf("bad listenfd\n");
        }

        printf("parent done\n");
        fflush(stdout);
    }
        


    //Fork off num processes
	// for ( 0 - num processes), fork(), give child id and 100/numprocesses money/widgets

	//Network Setup
	
	//Receiving Thread | Processing thread setup | Snapshot initializing thread id = 0

	//Message Passing with Logic


    return 0;
} 


