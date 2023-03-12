/******************************************************************************
 * NAME
 *   ui_functions.c
 *
 * DESCRIPTION
 *   functions that perform the user interface tasks
 *
 * COMMENTS
 * TODO: perguntar ao stor se é preciso abrir um socket novo sempre que queremos mandar qq coisa
 *       perguntar so stor como se descobre um id único para fzr o join
 *       pedir ajuda ao stor para por o server TCP a ouvir, ao mmo tempo que estamos a mandar cenas UDP e TCP (tem a ver com o select mas n sei mais)
 *       perguntar ao stor se qd fazemos leave temos de avisar os outros nós ou eles descobrem sozinhos
 *
 ******************************************************************************/
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <fcntl.h> // for open
#include <unistd.h> // for close

#include "udp_functions.h"

#define MAX_STR 100

struct addrinfo *get_udp_server_info(init_info_struct* info)
{
    struct addrinfo hints,*res;
    int errcode;
    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET;//IPv4
    hints.ai_socktype=SOCK_DGRAM;//UDP socket
    errcode=getaddrinfo(info->regIP,info->regUDP,&hints,&res);
    if(errcode!=0)/*error*/exit(1);
    return res;
}

int open_udp_socket()
{
    int fd=socket(AF_INET,SOCK_DGRAM,0);//UDP socket
    if(fd==-1)/*error*/perror("socket");
    return fd;
}

void send_message_udp(char* send_str, int fd, struct addrinfo* res)
{
    ssize_t n = sendto(fd,send_str,(int)strlen(send_str),0,res->ai_addr,res->ai_addrlen);
    if(n==-1)/*error*/perror("sendto");
}

void recv_message_udp(char* buffer, int fd)
{
    ssize_t n;
    struct sockaddr addr;
    socklen_t addrlen;

    addrlen=sizeof(addr);
    n=recvfrom(fd,buffer,128,0,&addr,&addrlen);
    if(n==-1)/*error*/exit(1);
    buffer[n] = '\0';
    printf("echo: %s\n", buffer);
}