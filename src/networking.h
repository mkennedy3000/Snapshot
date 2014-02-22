//Networking header file

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int udp_listen(int sockfd, char * buf);
int udp_send(int sockfd, char * message, struct addrinfo * p);
int set_up_talk(char * port, struct addrinfo *p);
int set_up_listen(char * port);

