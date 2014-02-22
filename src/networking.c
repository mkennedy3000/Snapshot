//Networking.c
//Set up UDP sockets for talking and listening
//

#include "networking.h"

#define localhost "127.0.0.1"
#define max_buf_len 100

int udp_listen(int sockfd, char * buf){
    int num_bytes;

    if((num_bytes = recvfrom(sockfd, buf, max_buf_len-1, 0, NULL, NULL)) == -1){
        printf("listen failure\n");
        return -1;
    }
    buf[num_bytes] = '\0';
    
    return num_bytes;
}

int udp_send(int sockfd, char * message, struct addrinfo * p){
    int num_bytes;

    if((num_bytes = sendto(sockfd, message, strlen(message), 0, p->ai_addr, p->ai_addrlen)) == -1){
        printf("send failure\n");
        return -1;
    }

    return num_bytes;
}

int set_up_talk(char * port, struct addrinfo *p){
    int sockfd;
    struct addrinfo hints, *servinfo;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if((rv = getaddrinfo(localhost, port, &hints, &servinfo)) != 0){
        printf("setUpTalk getaddrinfo error: %s\n", gai_strerror(rv));
        return -1;
    }

    for(p = servinfo; p != NULL; p = p->ai_next){
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            continue;
        }
        break;
    }

    if(p == NULL){
        printf("setUpTalk failed to bind to socket\n");
        return -1;
    }

    freeaddrinfo(servinfo);

    return sockfd;
}

int set_up_listen(char * port){
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0){
        printf("setUpListen getaddrinfo error: %s\n", gai_strerror(rv));
        return -1;
    }

    for(p = servinfo; p != NULL; p = p->ai_next){
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            continue;
        }
        if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
            continue;
        }
        break;
    }

    if(p == NULL){
        printf("setUpListen failed to bind to socket\n");
        return -1;
    }

    freeaddrinfo(servinfo);

    return sockfd;
}

