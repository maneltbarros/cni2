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
    if(n_TCP!=0)/*error*/
    {
        perror("getaddrinfo");
        exit(1);
    }
    return res_TCP;
}

int open_tcp_socket()
{
    int fd_TCP=socket(AF_INET,SOCK_STREAM,0);//TCP socket
    if(fd_TCP==-1)//error
    {
        perror("socket");
        exit(1);
    }
    return fd_TCP;
}

void tcp_connect(int fd_TCP, struct addrinfo* res_TCP)
{
    ssize_t n_TCP=connect(fd_TCP,res_TCP->ai_addr,res_TCP->ai_addrlen);
    if(n_TCP==-1)/*error*/
    {
        perror("connect");
        //o que fzr aqui?
        exit(1);
    }
}

void send_tcp(char* send_str, int fd_TCP, char* buffer)
{
    char* ptr=strcpy(buffer,send_str);
    ssize_t nbytes,nleft,nwritten;

    nbytes=(int)strlen(send_str);
    nleft=nbytes;
    while(nleft>0)
    {
        nwritten=write(fd_TCP,ptr,nleft);
        if(nwritten<=0)
        {
            perror("write");
            exit(1);
        }
        nleft-=nwritten;
        ptr+=nwritten;
        printf("send wrote: %s\n", buffer);
    }
}

int open_tcp_server(char* IP, char* TCP)
{
    struct addrinfo hints,*res;
    int fd,/*newfd,*/errcode; /*ssize_t n,nw;*/
    if((fd=socket(AF_INET,SOCK_STREAM,0))==-1)//error
    {
        perror("socket");
        exit(1);
    }
    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET;//IPv4
    hints.ai_socktype=SOCK_STREAM;//TCP socket
    hints.ai_flags=AI_PASSIVE;
    if((errcode=getaddrinfo(IP,TCP,&hints,&res))!=0)/*error*/
    {
        perror("getaddrinfo");
        exit(1);
    }
    if(bind(fd,res->ai_addr,res->ai_addrlen)==-1)/*error*/
    {
        perror("bind");
        exit(1);
    }
    if(listen(fd,100)==-1)/*error*/
    {
        perror("listen");
        exit(1);
    }
    return fd;
}

int accept_connection(int fd)
{
    int newfd;
    struct sockaddr addr; socklen_t addrlen;
    addrlen = sizeof(addr);
    if((newfd=accept(fd,&addr,&addrlen))==-1)
    /*error*/
    {
        perror("accept");
        exit(1);
    }
    return newfd;
}

void receive_and_send_tcp(int newfd, int* fdes, init_info_struct* info, node_info_struct* node, fd_set* inputs)
{
    ssize_t n,nw;
    char *ptr,buffer[128] = {0,};
    strcpy(buffer, " ");
    int first_time = 1;
    int scanf_return = 0;
    char error_msg[100];
    strcpy(error_msg, "received invalid message");

    while((n=read(newfd,buffer,128))!=0)
    {
        if(n==-1)/*error*/
        {
            perror("read");
            exit(1);
        }
        if(first_time == 1)first_time = 0;
        printf("read:%s\n", buffer);
        printf("n:%ld\n", n);
        printf("second to last char: %d; last_char:%d\n", (int)buffer[strlen(buffer)-1], (int)buffer[strlen(buffer)]);
        if((int)buffer[n-1] == 10)/*reading completed*/break;
    }
    if(first_time)
    {
        printf("alguem bazou\n");
        close(newfd);
        FD_CLR(newfd, inputs);
        int id_do_gajo = -1;
        for(int i = 0; i< 100; ++i)
        {
            if(fdes[i] == newfd)id_do_gajo = i;
        }
        fdes[id_do_gajo] = -1;
        if(id_do_gajo == -1 && node->ext_fd == newfd)id_do_gajo = atoi(node->ext);
        printf("foi este gajo:%d;\n", id_do_gajo);
        fix_topology(id_do_gajo, node, info, fdes, inputs);
        printf("vai bazar do receive_and_send\n");
        return;
    }

    char* str1 = (char*)malloc(MAX_STR);
    char* str2 = (char*)malloc(MAX_STR);
    char* str3 = (char*)malloc(MAX_STR);
    char* str4 = (char*)malloc(MAX_STR);

    char* return_value = (char*)malloc(MAX_STR);

    scanf_return = sscanf(buffer, "%s %s %s %s", str1, str2, str3, str4);

    printf("recv instruction:%s;\n", str1);

    if(strcmp(str1, "NEW") == 0)
    {
        if(is_valid_id(str2) && scanf_return > 3)
        {
            return_value = NEW(str2, str3, str4, node, info, return_value, fdes, newfd);
        }
        else
        {
            printf("%s\n", error_msg);
        }
    }
    else if(strcmp(str1, "EXTERN") == 0)
    {
        if(is_valid_id(str2) && scanf_return > 3)
        {
            return_value = EXTERN(str2, str3, str4, node, return_value);
        }
        else
        {
            printf("%s\n", error_msg);
        }
    }
    else if(strcmp(str1, "QUERY") == 0)
    {
        if(is_valid_id(str2) && is_valid_id(str3) && scanf_return > 3 && strcmp(str2, str3) != 0)
        {
            return_value = QUERY(str2, str3, str4, node, return_value, fdes, newfd);
        }
        else
        {
            printf("%s\n", error_msg);
        }
    }
    else if(strcmp(str1, "CONTENT") == 0)
    {
        if(is_valid_id(str2) && is_valid_id(str3) && scanf_return > 3 && strcmp(str2, str3) != 0)
        {
            return_value=CONTENT(str2,str3, str4, node, return_value, fdes, newfd);
        }
        else
        {
            printf("%s\n", error_msg);
        }
    }
    else if(strcmp(str1, "NOCONTENT") == 0)
    {
        if(is_valid_id(str2) && is_valid_id(str3) && scanf_return > 3 && strcmp(str2, str3) != 0)
        {
            return_value=N_CONTENT(str2,str3, str4, node, return_value, fdes, newfd);
        }
        else
        {
            printf("%s\n", error_msg);
        }
    }
    else if(strcmp(str1, "WITHDRAW") == 0)
    {
        if(atoi(str2) < 100 && atoi(str2) >= 0)     node->table[atoi(str2)] = -1;
    }
    else
    {
        printf("unknown message\n");
    }
    

    if(strcmp(return_value, "0") != 0)
    {
        strcpy(buffer, return_value);
        ptr=&buffer[0];
        n = strlen(buffer);
        while(n>0){if((nw=write(newfd,ptr,n))<=0)/*error*/
        {
            perror("write");
            exit(1);
        }
        printf("wrote:%s\n", ptr);
        n-=nw; ptr+=nw;}
    }
}

void fix_topology(int id_do_gajo, node_info_struct* node, init_info_struct* info, int* fdes, fd_set* inputs)
{
    char id_str[5];
    id_str[0] = id_do_gajo/10;
    id_str[1] = id_do_gajo%10;
    id_str[2] = '\0';
    char send_str[100];
    char buffer[128];
    strcpy(send_str, "0");

    node->table[id_do_gajo] = -1;

    for(int i = 0; i < node->num_intr;++i)
    {
        if(id_do_gajo == atoi(node->intr[i]->id))       //remove from intr list
        {
            printf("ele era um no interno:%d;\n", i);
            free(node->intr[i]);
            fdes[id_do_gajo] = -1;
            if(node->num_intr > 1)
            {
                node->intr[i] = node->intr[node->num_intr - 1];
            }
            node->num_intr--;
            printf("ja limpou o interno\n");

            ////////////////////-----------------mandar WITHDRAWs 
            sprintf(send_str, "%s %s\n", "WITHDRAW", id_str);

            for (int i = 0; i < node->num_intr; i++)
            {
                send_tcp(send_str, fdes[atoi(node->intr[i]->id)], buffer);
            }
            if (strcmp(node->ext, node->id) != 0)
            {
                send_tcp(send_str, node->ext_fd, buffer);
            }
            ////////////////////////////////////////////////////
            return;
        }
    }
    if(strcmp(node->bck, node->id) != 0)
    {
        printf("ele era o no externo\n");
        strcpy(node->ext, node->bck);
        strcpy(node->ext_IP, node->bck_IP);
        strcpy(node->ext_TCP, node->bck_TCP);

        int fd_TCP=open_tcp_socket();
        struct addrinfo *res_TCP = get_tcp_server_info(node->ext_IP, node->ext_TCP);

        node->ext_fd = fd_TCP;

        printf("vou fzr connect com este gajo:%s %s %s;\n", node->ext, node->ext_IP, node->ext_TCP);
        tcp_connect(fd_TCP, res_TCP);
        sprintf(send_str, "%s %s %s %s\n", "NEW", node->id, info->IP, info->TCP);
        printf("vou fzr send para o ext\n");
        send_tcp(send_str, node->ext_fd, buffer);
        FD_SET(fd_TCP, inputs);
        node->table[atoi(node->ext)] = atoi(node->ext);

        sprintf(send_str, "%s %s %s %s\n", "EXTERN", node->ext, node->ext_IP, node->ext_TCP);
        for(int i = 0; i < node->num_intr;++i)
        {
            printf("vou fzr send para o interno:%d;", atoi(node->intr[i]->id));
            send_tcp(send_str, fdes[atoi(node->intr[i]->id)], buffer);
        }

        strcpy(node->bck, node->id);    //assumo que sou ancora, para que se nao receber nenhum EXTERN ficar tudo fixe
        strcpy(node->bck_IP, info->IP);
        strcpy(node->bck_TCP, info->TCP);
    }
    else if(node->num_intr > 0)
    {
        printf("ele era um no ancora e ha mais nos\n");
        strcpy(node->ext, node->intr[0]->id);
        strcpy(node->ext_IP, node->intr[0]->IP);
        strcpy(node->ext_TCP, node->intr[0]->TCP);

        node->ext_fd = fdes[atoi(node->intr[0]->id)];

        sprintf(send_str, "%s %s %s %s\n", "EXTERN", node->ext, node->ext_IP, node->ext_TCP);
        for(int i = 0; i < node->num_intr;++i)
        {
            printf("vou fzr send para o interno:%d;", atoi(node->intr[i]->id));
            send_tcp(send_str, fdes[atoi(node->intr[i]->id)], buffer);
        }

        free(node->intr[0]);
        if(node->num_intr > 1)
        {
            node->intr[0] = node->intr[node->num_intr - 1];
        }
        node->num_intr--;
    }
    else
    {
        printf("ele era um no ancora e nao ha mais nos\n");
        strcpy(node->ext, node->id);
        strcpy(node->ext_IP, info->IP);
        strcpy(node->ext_TCP, info->TCP);
        strcpy(node->bck, node->id);
        strcpy(node->bck_IP, info->IP);
        strcpy(node->bck_TCP, info->TCP);
        //node->ext_fd = ?
    }

    sprintf(send_str, "%s %s\n", "WITHDRAW", id_str);

    for (int i = 0; i < node->num_intr; i++)
    {
        send_tcp(send_str, fdes[atoi(node->intr[i]->id)], buffer);
    }
    if (strcmp(node->ext, node->id) != 0)
    {
        send_tcp(send_str, node->ext_fd, buffer);
    }

}

char* NEW(char* id, char* IP, char* TCP, node_info_struct* node, init_info_struct * info, char* return_value, int* fdes, int newfd)
{
    node->table[atoi(id)] = atoi(id);

    if(strcmp(node->ext, node->id) == 0)
    {
        strcpy(node->ext, id);
        strcpy(node->ext_IP, IP);
        strcpy(node->ext_TCP, TCP);
        node->ext_fd = newfd;
        strcpy(node->bck, node->id);
        strcpy(node->bck_IP, info->IP);
        strcpy(node->bck_TCP, info->TCP);
        strcpy(return_value, "0");
    }
    else
    {
        fdes[atoi(id)] = newfd;
        node->intr[node->num_intr] = init_internal_node(id, IP, TCP);
        node->num_intr++;
        sprintf(return_value, "%s %s %s %s\n", "EXTERN", node->ext, node->ext_IP, node->ext_TCP);
    }
    return return_value;
}

char* EXTERN(char* bck, char* IP, char* TCP, node_info_struct* node, char* return_value)
{
    strcpy(node->bck, bck);
    strcpy(node->bck_IP, IP);
    strcpy(node->bck_TCP, TCP);
    strcpy(return_value, "0");
    return return_value;
}

char* QUERY(char* dest, char* orig, char* name, node_info_struct* node, char* return_value, int* fdes, int newfd)
{
    int check=0;
    if(strcmp(dest,orig)==0)
    {
        printf("Invalid message");
        return_value = "0";
        return return_value;
    }
    if (strcmp(dest,node->id)==0)
    {
       for (int i = 0; i < node->num_content; i++)//ver se o content existe
       {
            if (strcmp(name,node->contents[i])==0)
            {
                if (node->num_content == 1)
                {
                    node->num_content--;
                }
                else
                {
                    strcpy(node->contents[i],node->contents[node->num_content-1]);
                    node->num_content--;
                }
                check=1;
                break;
            }
       }
        if(check==1)
        sprintf(return_value, "%s %s %s %s\n", "CONTENT", orig, dest, name); 
        else
        sprintf(return_value, "%s %s %s %s\n", "NOCONTENT", orig, dest, name);
    }
    else
    {
        char buffer[128];
        strcpy(buffer, " ");
        char send_str[100];
        if ((node->table[atoi(dest)])!=-1 && (node->table[atoi(dest)])!=atoi(node->ext))
        {
            sprintf(send_str, "%s %s %s %s\n", "QUERY", dest, orig, name);
            send_tcp(send_str,fdes[node->table[atoi(dest)]],buffer);
            return_value = "0";
        }
        if ((node->table[atoi(dest)])!=-1 && (node->table[atoi(dest)])==atoi(node->ext))
        {
            sprintf(send_str, "%s %s %s %s\n", "QUERY", dest, orig, name);
            send_tcp(send_str,node->ext_fd,buffer);
            return_value = "0";  
        }
        
        if(node->table[atoi(dest)] == -1)
        {
            for (int i = 0; i < 100; i++)
            {
                if (fdes[i]!=-1 && fdes[i]!=newfd) 
                {
                    sprintf(send_str, "%s %s %s %s\n", "QUERY", dest, orig, name);
                    send_tcp(send_str,fdes[i],buffer);
                }
            }
            return_value = "0";
        }       
    }
    update_intr(fdes, newfd, node, orig);
    update_ext(newfd, node, orig);
    return return_value; 
}

char* CONTENT(char* dest, char* orig, char* name, node_info_struct* node, char* return_value, int* fdes, int newfd)
{
    char buffer[128];
    strcpy(buffer, " ");
    char send_str[100];

    if (strcmp(dest,(node->id))==0) //se nos somos o destino
    {
        strcpy(node->contents[node->num_content],name);
        node->num_content++;
        printf("Content %s added to the node\n",name);
    }
    else if((node->table[atoi(dest)]!= atoi(node->ext)))        //se nao for para mandar para o externo
    {
        sprintf(send_str, "%s %s %s %s\n", "CONTENT", dest, orig, name);
        send_tcp(send_str,fdes[node->table[atoi(dest)]],buffer);
    }
    else    //se for para mandar para o externo
    {
        sprintf(send_str, "%s %s %s %s\n", "CONTENT", dest, orig, name);
        send_tcp(send_str,node->ext_fd,buffer);
    }
    return_value = "0";
    update_intr(fdes, newfd, node, orig);
    update_ext(newfd, node, orig);
    return return_value; 
}

char* N_CONTENT(char* dest, char* orig, char* name, node_info_struct* node, char* return_value, int* fdes, int newfd)
{
    char buffer[128];
    strcpy(buffer, " ");
    char send_str[100];

    if (strcmp(dest,(node->id))==0)
    {
        printf("Content %s does not exist\n",name);
    }
    else if((node->table[atoi(dest)]!= atoi(node->ext)) )
    {
        printf("NOCONTENT %d %d\n",atoi(dest), node->table[atoi(dest)]);
        sprintf(send_str, "%s %s %s %s\n", "NOCONTENT", dest, orig, name);
        send_tcp(send_str,fdes[node->table[atoi(dest)]],buffer);
    }
    else
    {
        sprintf(send_str, "%s %s %s %s\n", "NOCONTENT", dest, orig, name);
        send_tcp(send_str,node->ext_fd,buffer);
    }
    update_intr(fdes, newfd, node, orig);
    update_ext(newfd, node, orig);
    return_value = "0";
    return return_value;   
}

void update_intr(int* fdes, int newfd, node_info_struct* node, char* orig)
{
    if (newfd != -1)
    {
        for (int i = 0; i < 100; i++)
        {
            if (fdes[i]==newfd) //nó que nos mandou o query
            {
                node->table[atoi(orig)]=i;     //atualiza a tabela de expedição
            }  
        } 
    }  
}

void update_ext(int newfd, node_info_struct* node, char* orig)
{
    if ((newfd == node->ext_fd) && (newfd != -1))
    {
        node->table[atoi(orig)]=atoi(node->ext);     //atualiza a tabela de expedição
    } 
}
