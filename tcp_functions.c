/******************************************************************************
 * NAME
 *   ui_functions.c
 *
 * DESCRIPTION
 *   functions that perform the user interface tasks
 *
 * COMMENTS
 * TODO: perguntar ao stor o que se faz quando somos o orig e recebemos un content ou nocontent (get)   
 *       como e que eu sei que alguem fez leave (quando eles fazem close(fd), recebemos qq coisa?))
 *          - ent nao posso fzr close(fd) quando sou cliente certo?
 *       é preciso o valgrind estar bom? - como e que se resolve aquela shit do buffer
 *       o que e preciso fzr no leave?
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

#include "tcp_functions.h"

#define MAX_STR 100

struct addrinfo *get_tcp_server_info(char* bootIP, char* bootTCP)
{
    struct addrinfo hints_TCP,*res_TCP;
    int n_TCP;
    memset(&hints_TCP,0,sizeof hints_TCP);
    hints_TCP.ai_family=AF_INET;//IPv4
    hints_TCP.ai_socktype=SOCK_STREAM;//TCP socket
    n_TCP=getaddrinfo(bootIP, bootTCP, &hints_TCP,&res_TCP);
    if(n_TCP!=0)/*error*/perror("getaddrinfo");
    return res_TCP;
}

int open_tcp_socket()
{
    int fd_TCP=socket(AF_INET,SOCK_STREAM,0);//TCP socket
    if(fd_TCP==-1)exit(1);//error
    return fd_TCP;
}

void tcp_connect(int fd_TCP, struct addrinfo* res_TCP)
{
    ssize_t n_TCP=connect(fd_TCP,res_TCP->ai_addr,res_TCP->ai_addrlen);
    if(n_TCP==-1)/*error*/perror("connect");
}

void send_and_receive_tcp(char* send_str, int fd_TCP, char* buffer)
{
    char* ptr=strcpy(buffer,send_str);
    ssize_t nbytes,nleft,nwritten,nread, total_read = 0;

    nbytes=(int)strlen(send_str);
    nleft=nbytes;
    while(nleft>0)
    {
        nwritten=write(fd_TCP,ptr,nleft);
        if(nwritten<=0)/*error*/exit(1);
        nleft-=nwritten;
        ptr+=nwritten;
        printf("send_and_receive wrote: %s\n", buffer);
    }

    strcpy(buffer, " ");
    ptr = &buffer[0];
    while((nread=read(fd_TCP,ptr,128)) != 0)
    {
        if(nread==-1)/*error*/exit(1);
        ptr+=nread;
        total_read += nread;
        printf("nread:%ld;\n", nread);
        if((int)buffer[nread-1] == 10)/*reading completed*/break;
    }
    buffer[total_read] = '\0';
    printf("received: %s\n", buffer);
}

int open_tcp_server(char* IP, char* TCP)
{
    struct addrinfo hints,*res;
    int fd,/*newfd,*/errcode; /*ssize_t n,nw;*/
    if((fd=socket(AF_INET,SOCK_STREAM,0))==-1)perror("socket");//error
    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET;//IPv4
    hints.ai_socktype=SOCK_STREAM;//TCP socket
    hints.ai_flags=AI_PASSIVE;
    if((errcode=getaddrinfo(IP,TCP,&hints,&res))!=0)/*error*/exit(1);
    if(bind(fd,res->ai_addr,res->ai_addrlen)==-1)/*error*/perror("bind");
    if(listen(fd,5)==-1)/*error*/perror("listen");
    return fd;
}

int accept_connection(int fd)
{
    int newfd;
    struct sockaddr addr; socklen_t addrlen;
    addrlen = sizeof(addr);
    if((newfd=accept(fd,&addr,&addrlen))==-1)
    /*error*/exit(1);
    return newfd;
}

void receive_and_send_tcp(int newfd, int* fdes, init_info_struct* info, node_info_struct* node)
{
    ssize_t n,nw;
    char *ptr,buffer[128];
    strcpy(buffer, " ");
    int first_time = 1;

    while((n=read(newfd,buffer,128))!=0)
    {
        if(n==-1)/*error*/exit(1);
        if(first_time == 1)first_time = 0;
        printf("read:%s\n", buffer);
        printf("n:%ld\n", n);
        printf("second to last char: %d; last_char:%d\n", (int)buffer[strlen(buffer)-1], (int)buffer[strlen(buffer)]);
        if((int)buffer[n-1] == 10)/*reading completed*/break;
    }
    if(first_time)
    {
        close(newfd);
        return;
    }

    char* str1 = (char*)malloc(MAX_STR);
    char* str2 = (char*)malloc(MAX_STR);
    char* str3 = (char*)malloc(MAX_STR);
    char* str4 = (char*)malloc(MAX_STR);

    char* return_value = (char*)malloc(MAX_STR);

    sscanf(buffer, "%s %s %s %s", str1, str2, str3, str4);

    printf("recv instruction:%s;\n", str1);

    if(strcmp(str1, "NEW") == 0)
    {
        return_value = NEW(str2, str3, str4, node, return_value, fdes, newfd);
    }
    else if(strcmp(str1, "QUERY") == 0)
    {
        return_value = QUERY(str2, str3, str4, node, return_value, fdes, newfd);
    }
    else if(strcmp(str1, "CONTENT") == 0)
    {
        return_value=CONTENT(str2,str3, str4, node, return_value, fdes, newfd);
    }
    else if(strcmp(str1, "NOCONTENT") == 0)
    {
    }
    else if(strcmp(str1, "WITHDRAW") == 0)
    {
        node->table[atoi(str2)] = -1;
    }

    if(strcmp(return_value, "0") != 0)
    {
        strcpy(buffer, return_value);
        ptr=&buffer[0];
        n = strlen(buffer);
        while(n>0){if((nw=write(newfd,ptr,n))<=0)/*error*/exit(1);
        printf("wrote:%s\n", ptr);
        n-=nw; ptr+=nw;}
    }
}

char* NEW(char* id, char* IP, char* TCP, node_info_struct* node, char* return_value, int* fdes, int newfd)
{
    fdes[atoi(id)] = newfd;

    if(strcmp(node->ext, node->id) == 0)
    {
        strcpy(node->ext, id);
        strcpy(node->bck, node->id);
        strcpy(return_value, "0");
    }
    else
    {
        node->intr[node->num_intr] = init_internal_node(id, IP, TCP);
        node->num_intr++;
        sprintf(return_value, "%s %s %s %s\n", "EXTERN", node->ext, node->ext_IP, node->ext_TCP);
    }
    return return_value;
}

char* QUERY(char* dest, char* orig, char* name, node_info_struct* node, char* return_value, int* fdes, int newfd)
{
    int check=0;
    if (strcmp(dest,node->id)==0)
    {
       for (int i = 0; i < 100; i++)//ver se o content existe
       {
            if (fdes[i]==newfd) //nó que nos mandou o query
            {
                node->table[atoi(orig)]=i;     //atualiza a tabela de expedição
            }  
            if (strcmp(name,node->contents[i])==0)
            {
                check=1;
                break;
            }
            else
            {
                check=check;
            } 
       }
        if(check==1)
        sprintf(return_value, "%s %s %s %s\n", "CONTENT", orig, dest, name); 
        else
        sprintf(return_value, "%s %s %s %s\n", "NONCONTENT", orig, dest, name);
    }
    else
    {
            char buffer[128+1];
            char buffer1[128+1];

            strcpy(buffer, " ");
            strcpy(buffer1, " ");
            if ((node->table[atoi(dest)])!=-1)
            {
                char* send_str = (char*)malloc(MAX_STR);
                strcpy(send_str, "QUERY");
                strcat(send_str, " ");
                strcat(send_str, orig);
                strcat(send_str, " ");
                strcat(send_str, dest);
                strcat(send_str, " ");
                strcat(send_str, name);
                strcat(send_str, "\n");
                send_and_receive_tcp(send_str,fdes[node->table[atoi(dest)]],buffer);
                check=1;
                //fazer return_value=0??
            }
            else
            {
                for (int i = 0; i < 100; i++)
                {
                    if (fdes[i]!=-1 && fdes[i]!=newfd && check==0) //alterar fdes para -1
                    {
                        char* send_str = (char*)malloc(MAX_STR);
                        strcpy(send_str, "QUERY");
                        strcat(send_str, " ");
                        strcat(send_str, dest);
                        strcat(send_str, " ");
                        strcat(send_str, orig);
                        strcat(send_str, " ");
                        strcat(send_str, name);
                        strcat(send_str, "\n");
                        send_and_receive_tcp(send_str,fdes[i],buffer);
                    }
                    if (fdes[i]==newfd) //nó que nos mandou o query
                    {
                        node->table[atoi(orig)]=i;     //atualiza a tabela de expedição
                    }
                }

                if (newfd == node->ext_fd)
                {
                    node->table[atoi(orig)]=atoi(node->ext);     //atualiza a tabela de expedição
                }
                else
                {
                    char* send_str = (char*)malloc(MAX_STR);
                    strcpy(send_str, "QUERY");
                    strcat(send_str, " ");
                    strcat(send_str, dest);
                    strcat(send_str, " ");
                    strcat(send_str, orig);
                    strcat(send_str, " ");
                    strcat(send_str, name);
                    strcat(send_str, "\n");
                    send_and_receive_tcp(send_str,node->ext_fd,buffer1);
                }
            }
            

                
    }
    return return_value; 
}

char* CONTENT(char* dest, char* orig, char* name, node_info_struct* node, char* return_value, int* fdes, int newfd)
{
    if (strcmp(dest,(node->id))==0)
    {
        /* o que fazer? */
    }
    else if((atoi(node->ext))!= node->table[atoi(dest)])
    {
        char buffer[128+1];
        strcpy(buffer, " ");

        char* send_str = (char*)malloc(MAX_STR);
        strcpy(send_str, "CONTENT");
        strcat(send_str, " ");
        strcat(send_str, dest);
        strcat(send_str, " ");
        strcat(send_str, orig);
        strcat(send_str, " ");
        strcat(send_str, name);
        strcat(send_str, "\n");
        send_and_receive_tcp(send_str,fdes[node->table[atoi(dest)]],buffer);
    }
    else
    {
        char buffer[128+1];
        strcpy(buffer, " ");

        char* send_str = (char*)malloc(MAX_STR);
        strcpy(send_str, "CONTENT");
        strcat(send_str, " ");
        strcat(send_str, dest);
        strcat(send_str, " ");
        strcat(send_str, orig);
        strcat(send_str, " ");
        strcat(send_str, name);
        strcat(send_str, "\n");
        send_and_receive_tcp(send_str,node->ext_fd,buffer);
    }
    
    
}

char* NONCONTENT(char* dest, char* orig, char* name, node_info_struct* node, char* return_value, int* fdes, int newfd)
{
    if (strcmp(dest,(node->id))==0)
    {
        /* o que fazer? */
    }
    else if((atoi(node->ext))!= node->table[atoi(dest)])
    {
        char buffer[128+1];
        strcpy(buffer, " ");

        char* send_str = (char*)malloc(MAX_STR);
        strcpy(send_str, "NONCONTENT");
        strcat(send_str, " ");
        strcat(send_str, dest);
        strcat(send_str, " ");
        strcat(send_str, orig);
        strcat(send_str, " ");
        strcat(send_str, name);
        strcat(send_str, "\n");
        send_and_receive_tcp(send_str,fdes[node->table[atoi(dest)]],buffer);
    }
    else
    {
        char buffer[128+1];
        strcpy(buffer, " ");

        char* send_str = (char*)malloc(MAX_STR);
        strcpy(send_str, "NONCONTENT");
        strcat(send_str, " ");
        strcat(send_str, dest);
        strcat(send_str, " ");
        strcat(send_str, orig);
        strcat(send_str, " ");
        strcat(send_str, name);
        strcat(send_str, "\n");
        send_and_receive_tcp(send_str,node->ext_fd,buffer);
    }
    
    
}
