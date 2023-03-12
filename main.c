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

#include "main_functions.h"
#include "ui_functions.h"
#include "udp_functions.h"

#define MAX_STR 100


int main(int argc, char *argv[])
{

    init_info_struct* info = allocate_info();

    node_info_struct* node = allocate_node();

    info = process_init_input(info, argc, argv);

    //printf("%s %s %s %s\n", info->IP, info->TCP, info->regIP, info->regUDP);

    char* str1 = (char*)malloc(MAX_STR);
    char* str2 = (char*)malloc(MAX_STR);
    char* str3 = (char*)malloc(MAX_STR);
    char* str4 = (char*)malloc(MAX_STR);
    char* str5 = (char*)malloc(MAX_STR);
    char* str6 = (char*)malloc(MAX_STR);

    char return_value = '0';
    char in_str[128];

    fd_set inputs, testfds;
    struct timeval timeout;

    int out_fds;

    char* return_fgets;

    struct addrinfo *res_udp = get_udp_server_info(info);

    int fd = open_tcp_server(info->IP, info->TCP);

    FD_ZERO(&inputs); // Clear inputs
    FD_SET(0,&inputs); // Set standard input channel on
    FD_SET(fd,&inputs); // Set fd channel on
    printf("Size of fd_set: %ld\n",sizeof(fd_set));
    printf("Value of FD_SETSIZE: %d\n",FD_SETSIZE);

    int fdes[100];

    for(int i = 0; i < 100; ++i)fdes[i]=0;

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
                        node->ext_fd = join(str2, str3, res_udp, info, node);
                    }
                    else if(strcmp(str1, "djoin") == 0)
                    {
                        node->ext_fd = djoin(str2, str3, str4, str5, str6, res_udp, info, node);
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
                        return_value = get_fctn(str2, str3, info, node);
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
                        exit_fctn(str1, str2, str3, str4, str5, str6, res_udp, info, fdes);
                    }
                    else
                    {
                        perror("unknown command");
                    }
                }
                else if(FD_ISSET(fd,&testfds))
                {
                    int newfd = accept_connection(fd);
                    receive_and_send_tcp(newfd, fdes, info, node);
                }
        }
    }
}
/*freeaddrinfo(res);*/

