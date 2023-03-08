/******************************************************************************
 * NAME
 *   ui_functions.c
 *
 * DESCRIPTION
 *   functions that perform the user interface tasks
 *
 * COMMENTS
 * TODO: perguntar so stor como se descobre um id único para fzr o join
 *
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "ui_functions.h"

#define MAX_STR 100

char join(char* net, char* id, struct addrinfo* res, init_info_struct* info, node_info_struct* node)
{
    ssize_t n;
    int fd;
    struct sockaddr addr;
    socklen_t addrlen;
    char buffer[128+1];

    char* send_str = (char*)malloc(MAX_STR);

    

    id_unique_struct* id_struct = (id_unique_struct*)malloc(sizeof(id_unique_struct));

    id_struct->id_unique_bootid = (char*)calloc(5, sizeof(char));
    id_struct->id_unique_bootIP = (char*)calloc(MAX_STR, sizeof(char));
    id_struct->id_unique_bootTCP = (char*)calloc(MAX_STR, sizeof(char));

    strcpy(id_struct->id_unique_bootid, "0");
    strcpy(id_struct->id_unique_bootIP, "0");
    strcpy(id_struct->id_unique_bootTCP, "0");

    strcpy(send_str, "NODES ");
    strcat(send_str, net);

    printf("%s\n", send_str);

    fd=socket(AF_INET,SOCK_DGRAM,0);//UDP socket
    if(fd==-1)/*error*/perror("socket");
    n=sendto(fd,send_str,(int)strlen(send_str) + 1,0,res->ai_addr,res->ai_addrlen);
    if(n==-1)/*error*/perror("sendto");

    printf("coiso\n");

    addrlen=sizeof(addr);
    n=recvfrom(fd,buffer,128 + 1,0,&addr,&addrlen);
    if(n==-1)/*error*/exit(1);
    buffer[n] = '\0';
    printf("echo: %s\n", buffer);
    close(fd);

    printf("coiso2\n");

    id_struct = id_unique(buffer, id_struct);
    
    printf("%s %s %s\n", id_struct->id_unique_bootid, id_struct->id_unique_bootIP, id_struct->id_unique_bootTCP);

    /////////////////////-----------------send REG UDP

    strcpy(send_str, "REG ");
    strcat(send_str, net);
    strcat(send_str, " ");
    strcat(send_str, id);
    strcat(send_str, " ");
    strcat(send_str, info->IP);
    strcat(send_str, " ");
    strcat(send_str, info->TCP);

    printf("%s\n", send_str);

    fd=socket(AF_INET,SOCK_DGRAM,0);//UDP socket
    if(fd==-1)/*error*/perror("socket");
    n=sendto(fd,send_str,(int)strlen(send_str),0,res->ai_addr,res->ai_addrlen);
    if(n==-1)/*error*/perror("sendto");

    addrlen=sizeof(addr);
    n=recvfrom(fd,buffer,128,0,&addr,&addrlen);
    if(n==-1)/*error*/exit(1);
    buffer[n] = '\0';
    printf("echo: %s\n", buffer);
    close(fd);
    ////////////////////////////////////////////////////////////////

    return djoin(net, id, id_struct->id_unique_bootid, id_struct->id_unique_bootIP, id_struct->id_unique_bootTCP, res, info, node);

}

char djoin(char* net, char* id, char* bootid, char* bootIP, char* bootTCP, struct addrinfo* res, init_info_struct* info, node_info_struct* node)
{
    char buffer[128+1];

    char* send_str = (char*)malloc(MAX_STR);

    /////////////////////---------------add node info
    strcpy(node->net, net);
    strcpy(node->id, id);
    strcpy(node->ext, bootid);
    strcpy(node->ext_IP, bootIP);
    strcpy(node->ext_TCP, bootTCP);
    ////////////////////////////////////////////////////////////

    

    printf("%s %s\n", bootIP, bootTCP);

    if(strcmp(id, bootid) != 0 && strcmp("0", bootid) != 0) //only open TCP session if net already has nodes
    {
        //////////////////////-------------------connect to TCP
        struct addrinfo hints_TCP,*res_TCP;
        int fd_TCP,n_TCP;

        fd_TCP=socket(AF_INET,SOCK_STREAM,0);//TCP socket
        if(fd_TCP==-1)exit(1);//error
        memset(&hints_TCP,0,sizeof hints_TCP);
        hints_TCP.ai_family=AF_INET;//IPv4
        hints_TCP.ai_socktype=SOCK_STREAM;//TCP socket
        n_TCP=getaddrinfo(bootIP, bootTCP, &hints_TCP,&res_TCP);
        if(n_TCP!=0)/*error*/perror("getaddrinfo");
        n_TCP=connect(fd_TCP,res_TCP->ai_addr,res_TCP->ai_addrlen);
        if(n_TCP==-1)/*error*/perror("connect");
        ////////////////////////////////////////////////////////

        //////////////////////----------------------send NEW TCP
        ssize_t nbytes,nleft,nwritten,nread;
        char *ptr;

        strcpy(send_str, "NEW ");
        strcat(send_str, " ");
        strcat(send_str, id);
        strcat(send_str, " ");
        strcat(send_str, info->IP);
        strcat(send_str, " ");
        strcat(send_str, info->TCP);
        strcat(send_str, "\n");

        ptr=strcpy(buffer,send_str);
        nbytes=(int)strlen(send_str);
        nleft=nbytes;
        while(nleft>0){nwritten=write(fd_TCP,ptr,nleft);
        if(nwritten<=0)/*error*/exit(1);
        nleft-=nwritten;
        ptr+=nwritten;}
        nleft=nbytes; ptr=buffer;
        while(nleft>0){nread=read(fd_TCP,ptr,nleft);
        if(nread==-1)/*error*/exit(1);
        else if(nread==0)break;//closed by peer
        nleft-=nread;
        ptr+=nread;}
        nread=nbytes-nleft;
        buffer[nread] = '\0';
        printf("echo: %s\n", buffer);
        close(fd_TCP);
        ////////////////////////////////////////////////////////////
    }

    return 's';
}

char create(char* name)
{
    return 's';
}

char delete_fctn(char* name)
{
    return 's';
}

char get_fctn(char* dest, char* name)
{
    return 's';
}

char show_topology()
{
    return 's';
}

char show_names()
{
    return 's';
}

char show_routing()
{
    return 's';
}

char leave(struct addrinfo* res, init_info_struct* info, node_info_struct* node)
{
    ssize_t n;
    int fd;
    struct sockaddr addr;
    socklen_t addrlen;
    char buffer[128+1];

    char* send_str = (char*)malloc(MAX_STR);

    strcpy(send_str, "UNREG ");
    strcat(send_str, node->net);
    strcat(send_str, " ");
    strcat(send_str, node->id);

    printf("%s\n", send_str);

    fd=socket(AF_INET,SOCK_DGRAM,0);//UDP socket
    if(fd==-1)/*error*/perror("socket");
    n=sendto(fd,send_str,(int)strlen(send_str),0,res->ai_addr,res->ai_addrlen);
    if(n==-1)/*error*/perror("sendto");

    addrlen=sizeof(addr);
    n=recvfrom(fd,buffer,128,0,&addr,&addrlen);
    if(n==-1)/*error*/exit(1);
    buffer[n] = '\0';
    printf("echo: %s\n", buffer);
    close(fd);

    return 's';
}

void exit_fctn(char* str1, char* str2, char* str3, char* str4, char* str5, char* str6, struct addrinfo* res, init_info_struct* info)
{
    free(str1);
    free(str2);
    free(str3);
    free(str4);
    free(str5);
    free(str6);
    free_info(info);
    freeaddrinfo(res);
    exit(0);
}