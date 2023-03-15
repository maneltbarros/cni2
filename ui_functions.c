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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close

#include "ui_functions.h"

#define MAX_STR 100

int join(char* net, char* id, struct addrinfo* res, init_info_struct* info, node_info_struct* node, int fd)
{
    char buffer[128+1];

    choose_node_struct* chosen_node = init_choose_node_struct(id, info);

    char* send_str = (char*)malloc(MAX_STR);

    strcpy(send_str, "NODES ");
    strcat(send_str, net);

    printf("%s\n", send_str);

    int fd_udp = open_udp_socket();
    send_message_udp(send_str, fd_udp, res);

    recv_message_udp(buffer, fd_udp);

    id = unique_id(buffer, id);

    chosen_node = choose_node(buffer, chosen_node);

    printf("chosen_node:%s %s %s\n", chosen_node->chosen_node_bootid, chosen_node->chosen_node_bootIP, chosen_node->chosen_node_bootTCP);

    strcpy(send_str, "REG ");
    strcat(send_str, net);
    strcat(send_str, " ");
    strcat(send_str, id);
    strcat(send_str, " ");
    strcat(send_str, info->IP);
    strcat(send_str, " ");
    strcat(send_str, info->TCP);

    send_message_udp(send_str, fd_udp, res);

    recv_message_udp(buffer, fd_udp);

    close(fd_udp);

    return djoin(net, id, chosen_node->chosen_node_bootid, chosen_node->chosen_node_bootIP, chosen_node->chosen_node_bootTCP, res, info, node, fd);

}

int djoin(char* net, char* id, char* bootid, char* bootIP, char* bootTCP, struct addrinfo* res, init_info_struct* info, node_info_struct* node, int fd)
{

    char* send_str = (char*)malloc(MAX_STR);

    int fd_TCP = fd;

    /////////////////////---------------add node info
    strcpy(node->net, net);
    strcpy(node->id, id);
    strcpy(node->ext, bootid);
    strcpy(node->ext_IP, bootIP);
    strcpy(node->ext_TCP, bootTCP);
    strcpy(node->bck, id);
    strcpy(node->bck_IP, info->IP);
    strcpy(node->bck_TCP, info->TCP);
    ////////////////////////////////////////////////////////////

    printf("%s %s\n", bootIP, bootTCP);

    if(strcmp(id, bootid) != 0) //only open TCP session if net already has nodes
    {
        //////////////////////-------------------connect to TCP
        fd_TCP=open_tcp_socket();
        struct addrinfo *res_TCP = get_tcp_server_info(bootIP, bootTCP);

        tcp_connect(fd_TCP, res_TCP);
        ////////////////////////////////////////////////////////

        //////////////////////----------------------send NEW TCP and receive EXTERN

        strcpy(send_str, "NEW");
        strcat(send_str, " ");
        strcat(send_str, id);
        strcat(send_str, " ");
        strcat(send_str, info->IP);
        strcat(send_str, " ");
        strcat(send_str, info->TCP);
        strcat(send_str, "\n");

        char buffer[128+1];

        send_and_receive_tcp(send_str, fd_TCP, buffer);

        char* str1 = (char*)malloc(MAX_STR);
        char* str2 = (char*)malloc(MAX_STR);
        char* str3 = (char*)malloc(MAX_STR);
        char* str4 = (char*)malloc(MAX_STR);

        strcpy(str2, "0");

        sscanf(buffer, "%s %s %s %s", str1, str2, str3, str4);

        if(strcmp(str2, "0") != 0)
        {
            strcpy(node->bck, str2);
            strcpy(node->bck_IP, str3);
            strcpy(node->bck_TCP, str4);
        }
        ////////////////////////////////////////////////////////////
    }

    return fd_TCP;
}

char create(char* name)
{
    return 's';
}

char delete_fctn(char* name)
{
    return 's';
}

char get_fctn(char* dest, char* name, init_info_struct* info, node_info_struct* node)
{
    char* send_str = (char*)malloc(MAX_STR);
    char buffer[128+1];

    //////////////////////-------------------connect to TCP
    struct addrinfo hints_TCP,*res_TCP;
    int fd_TCP,n_TCP;

    fd_TCP=socket(AF_INET,SOCK_STREAM,0);//TCP socket
    if(fd_TCP==-1)exit(1);//error
    memset(&hints_TCP,0,sizeof hints_TCP);
    hints_TCP.ai_family=AF_INET;//IPv4
    hints_TCP.ai_socktype=SOCK_STREAM;//TCP socket
    n_TCP=getaddrinfo(node->ext_IP, node->ext_TCP, &hints_TCP,&res_TCP);
    if(n_TCP!=0)/*error*/perror("getaddrinfo");
    n_TCP=connect(fd_TCP,res_TCP->ai_addr,res_TCP->ai_addrlen);
    if(n_TCP==-1)/*error*/perror("connect");
    ////////////////////////////////////////////////////////

    //////////////////////----------------------send NEW TCP
    ssize_t nbytes,nleft,nwritten,nread;
    char *ptr;

    strcpy(send_str, "QUERY ");
    strcat(send_str, " ");
    strcat(send_str, dest);
    strcat(send_str, " ");
    strcat(send_str, node->id);     //nos somos a 'orig'
    strcat(send_str, " ");
    strcat(send_str, name);
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

void exit_fctn(char* str1, char* str2, char* str3, char* str4, char* str5, char* str6, struct addrinfo* res, init_info_struct* info, int* fdes)
{
    for(int i = 0; i < 100; ++i)if(fdes[i] != 0)close(fdes[i]);
    //close(node->ext_fd);

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