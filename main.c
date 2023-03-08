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


#include "main_functions.h"
#include "ui_functions.h"

#define MAX_STR 100


int main(int argc, char *argv[])
{

    init_info_struct* info = (init_info_struct*)malloc(sizeof(init_info_struct));
    info->IP = (char*)malloc(MAX_STR);
    info->TCP = (char*)malloc(MAX_STR);
    info->regIP = (char*)malloc(MAX_STR);
    info->regUDP = (char*)malloc(MAX_STR);

    node_info_struct* node = (node_info_struct*)malloc(sizeof(node_info_struct));
    node->net = (char*)malloc(5);
    node->id = (char*)malloc(5);
    node->ext = (char*)malloc(5);
    node->ext_IP = (char*)malloc(MAX_STR);
    node->ext_TCP = (char*)malloc(MAX_STR);
    node->bck = (char*)malloc(5);
    node->intr = (char**)malloc(sizeof(char*));
    node->table = (char***)malloc(2*sizeof(char**));

    info = process_init_input(info, argc, argv);

    printf("%s %s %s %s\n", info->IP, info->TCP, info->regIP, info->regUDP);

    char in_str[128];

    fd_set inputs, testfds;
    struct timeval timeout;

    int out_fds;

    char* return_fgets;

    char* str1 = (char*)malloc(MAX_STR);
    char* str2 = (char*)malloc(MAX_STR);
    char* str3 = (char*)malloc(MAX_STR);
    char* str4 = (char*)malloc(MAX_STR);
    char* str5 = (char*)malloc(MAX_STR);
    char* str6 = (char*)malloc(MAX_STR);

    char return_value = '0';

    //////////////////////////////////////-----------------------------------getaddrinfo udp
    struct addrinfo hints_udp,*res_udp;
    int errcode;
    memset(&hints_udp,0,sizeof hints_udp);
    hints_udp.ai_family=AF_INET;//IPv4
    hints_udp.ai_socktype=SOCK_DGRAM;//UDP socket
    errcode=getaddrinfo(info->regIP,info->regUDP,&hints_udp,&res_udp);
    if(errcode!=0)/*error*/exit(1);
    /////////////////////////////////////////////////////////////////

    //////////////////////////////////-----------------------getaddrinfo + listen
    struct addrinfo hints,*res;
    int fd,newfd; ssize_t n,nw;
    struct sockaddr addr; socklen_t addrlen;
    char *ptr,buffer[128];
    if((fd=socket(AF_INET,SOCK_STREAM,0))==-1)exit(1);//error
    FD_SET(fd,&inputs); // Set fd channel on
    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET;//IPv4
    hints.ai_socktype=SOCK_STREAM;//TCP socket
    hints.ai_flags=AI_PASSIVE;
    if((errcode=getaddrinfo(info->IP,info->TCP,&hints,&res))!=0)/*error*/exit(1);
    if(bind(fd,res->ai_addr,res->ai_addrlen)==-1)/*error*/exit(1);
    if(listen(fd,5)==-1)/*error*/exit(1);
    //////////////////////////////////

    FD_ZERO(&inputs); // Clear inputs
    FD_SET(0,&inputs); // Set standard input channel on
    printf("Size of fd_set: %ld\n",sizeof(fd_set));
    printf("Value of FD_SETSIZE: %d\n",FD_SETSIZE);
    while(1)
    {
        testfds=inputs;
        memset((void *)&timeout,0,sizeof(timeout));
        timeout.tv_sec=60;

        out_fds=select(FD_SETSIZE,&testfds,(fd_set *)NULL,(fd_set *)NULL,(struct timeval *) &timeout);

        switch(out_fds)
        {
            case 0:
                printf("\n ---------------Timeout event-----------------\n");
                break;
            case -1:
                perror("select");
                exit(1);
            default:
                if(FD_ISSET(0,&testfds))
                {
                    if((return_fgets = fgets(in_str, 100, stdin)) == NULL)
                    {
                        //does nothing
                    }
                    printf("------------------------------Input at keyboard: %s\n",in_str);

                    sscanf(in_str, "%s %s %s %s %s %s", str1, str2, str3, str4, str5, str6);

                    if(strcmp(str1, "join") == 0)
                    {
                        return_value = join(str2, str3, res_udp, info, node);
                    }
                    else if(strcmp(str1, "djoin") == 0)
                    {
                        return_value = djoin(str2, str3, str4, str5, str6, res_udp, info, node);
                    }
                    else if(strcmp(str1, "create") == 0)
                    {
                        return_value = create(str2);
                    }
                    else if(strcmp(str1, "delete") == 0)
                    {
                        return_value = delete_fctn(str2);
                    }
                    else if(strcmp(str1, "get") == 0)
                    {
                        return_value = get_fctn(str2, str3);
                    }
                    else if(strcmp(str1, "show") == 0)
                    {
                        if(strcmp(str1, "topology") == 0)
                        {
                            return_value = show_topology();
                        }
                        else if(strcmp(str1, "names") == 0)
                        {
                            return_value = show_names();
                        }
                        else if(strcmp(str1, "routing") == 0)
                        {
                            return_value = show_routing();
                        }
                    }
                    else if(strcmp(str1, "st") == 0)
                    {
                        return_value = show_topology();
                    }
                    else if(strcmp(str1, "sn") == 0)
                    {
                        return_value = show_names();
                    }
                    else if(strcmp(str1, "sr") == 0)
                    {
                        return_value = show_routing();
                    }
                    else if(strcmp(str1, "leave") == 0)
                    {
                        return_value = leave(res_udp, info, node);
                    }
                    else if(strcmp(str1, "exit") == 0)
                    {
                        exit_fctn(str1, str2, str3, str4, str5, str6, res_udp, info);
                    }
                    else
                    {
                        perror("unknown command");
                    }
                }
                else if(FD_ISSET(fd,&testfds))
                {
                    if((newfd=accept(fd,&addr,&addrlen))==-1)
                    FD_SET(newfd,&inputs); // Set newfd channel on
                    /*error*/exit(1);
                    while((n=read(newfd,buffer,128))!=0){if(n==-1)/*error*/exit(1);
                    printf("read:%s\n", buffer);
                    ptr=&buffer[0];
                    while(n>0){if((nw=write(newfd,ptr,n))<=0)/*error*/exit(1);
                    printf("wrote:%s\n", ptr);
                    n-=nw; ptr+=nw;}
                    }
                }
        }

        printf("%c", return_value);
    }
}
/*freeaddrinfo(res);*/

