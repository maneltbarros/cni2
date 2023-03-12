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

int main(int argc, char *argv[])
{
    char net[100];

    init_info_struct* info = (init_info_struct*)malloc(sizeof(init_info_struct));
    info->regIP = (char*)malloc(100);
    info->regUDP = (char*)malloc(100);

    strcpy(info->regIP, "193.136.138.142");
    strcpy(info->regUDP, "59000");

    strcpy(net, argv[1]);

    struct addrinfo *res_udp = get_udp_server_info(info);

    for(int k = 0; k < 100;++k)
    {
        char* send_str = (char*)malloc(MAX_STR);
        char* id = (char*)malloc(MAX_STR);

        id[0] = k/10 + 48;
        id[1] = k%10 + 48;
        id[2] = '\0';

        strcpy(send_str, "UNREG ");
        strcat(send_str, net);
        strcat(send_str, " ");
        strcat(send_str, id);

        int fd = open_udp_socket();
        send_message_udp(send_str, fd, res_udp);
        close(fd);
    }
}