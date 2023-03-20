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
    char buffer[5000];

    int return_value = -1;

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

    printf("send_str:%s;", send_str);

    send_message_udp(send_str, fd_udp, res);

    recv_message_udp(buffer, fd_udp);

    close(fd_udp);

    return_value = djoin(net, id, chosen_node->chosen_node_bootid, chosen_node->chosen_node_bootIP, chosen_node->chosen_node_bootTCP, res, info, node, fd);

    free(chosen_node);

    return return_value;

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

    if(strcmp(id, bootid) != 0) //only open TCP session if net already has nodes
    {
        //////////////////////-------------------connect to TCP
        fd_TCP=open_tcp_socket();
        struct addrinfo *res_TCP = get_tcp_server_info(bootIP, bootTCP);

        tcp_connect(fd_TCP, res_TCP);
        ////////////////////////////////////////////////////////

        //////////////////////----------------------send NEW TCP

        strcpy(send_str, "NEW");
        strcat(send_str, " ");
        strcat(send_str, id);
        strcat(send_str, " ");
        strcat(send_str, info->IP);
        strcat(send_str, " ");
        strcat(send_str, info->TCP);
        strcat(send_str, "\n");

        char buffer[128+1];
        printf("agora vai fzr send\n");
        send_tcp(send_str, fd_TCP, buffer);
        node->table[atoi(node->ext)] = atoi(node->ext);
        ////////////////////////////////////////////////////////////
    }

    return fd_TCP;
}

char create(char* name, node_info_struct* node)
{
    strcpy(node->contents[node->num_content], name);
    node->num_content++;
    return 's';
}

char delete_fctn(char* name, node_info_struct* node)
{
    for (int i = 0; i < node->num_content; i++)
    {
        if (strcmp(node->contents[i], name) == 0)
        {
            if(node->num_content > 1)
            {
                strcpy(node->contents[i], node->contents[node->num_content]);
            }
            node->num_content--;
        } 
    }
    
    return 's';
}

char get_fctn(char* dest, char* name, init_info_struct* info, node_info_struct* node, int* fdes)
{
    char send_str[100];
    sprintf(send_str, "%s %s %s %s\n", "QUERY", dest, node->id, name);
    char buffer[128] = {0,};

    if (node->table[atoi(dest)]!=-1)
    {
        if (atoi(node->ext) == node->table[atoi(dest)])
        {
            send_tcp(send_str,node->ext_fd,buffer);
        }
        else
        {
            printf("Vou fazer send para o int: %d ;\n ", node->table[atoi(dest)]);
            send_tcp(send_str,fdes[node->table[atoi(dest)]],buffer);
        }
    return 's';
    }
    else
    {
        printf("vou fazer send para o ext:%s %s %s %d;\n", node->ext, node->ext_IP, node->ext_TCP, node->ext_fd);
        send_tcp(send_str,node->ext_fd,buffer);
        for(int j = 0; j< node->num_intr;++j)
        {
        if(fdes[j]!=-1)
        send_tcp(send_str,fdes[atoi(node->intr[j]->id)],buffer);
        }
    }
    


    return 's';
}

char show_topology(node_info_struct* node)
{
    printf("Vizinhos internos:\n");
    for(int i = 0; i < node->num_intr; ++i)
    {
        printf("%s %s %s\n", node->intr[i]->id, node->intr[i]->IP, node->intr[i]->TCP);
    }
    printf("\n");

    printf("Vizinho externo:\n");
    printf("%s %s %s\n", node->ext, node->ext_IP, node->ext_TCP);
    printf("\n");

    printf("Backup node:\n");
    printf("%s %s %s\n", node->bck, node->bck_IP, node->bck_TCP);
    printf("\n");

    return 's';
}

char show_names(node_info_struct* node)
{
    printf("Contents:\n");
    for (int i = 0; i < node->num_content; i++)
    {
        printf("%s\n", node->contents[i]);
    }
    printf("\n");
    
    return 's';
}

char show_routing(node_info_struct* node)
{
    printf("Routing:\n");
    for (int i = 0; i < 100; i++)
    {
        if(node->table[i] != -1)printf("%d -> %d\n", i, node->table[i]);
    }
    printf("\n");
    
    return 's';
}

char leave(struct addrinfo* res, init_info_struct* info, node_info_struct* node, int* fdes, int fd, fd_set* inputs)
{
    char buffer[128+1];

    char send_str[100];

    strcpy(send_str, "UNREG ");
    strcat(send_str, node->net);
    strcat(send_str, " ");
    strcat(send_str, node->id);

    printf("%s\n", send_str);

    int fd_udp = open_udp_socket();
    send_message_udp(send_str, fd_udp, res);

    recv_message_udp(buffer, fd_udp);
    close(fd_udp);

    for(int i = 0; i< 100; ++i)
    {
        if(fdes[i] != -1)
        {
            close(fdes[i]);
            FD_CLR(fdes[i], inputs);
        }
    }
    close(node->ext_fd);
    FD_CLR(node->ext_fd, inputs);
    reset_node(node);
    //close(fd);

    return 's';
}

void exit_fctn(char* str1, char* str2, char* str3, char* str4, char* str5, char* str6, struct addrinfo* res, init_info_struct* info, node_info_struct* node, int* fdes)
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
    free_node(node);
    freeaddrinfo(res);
    exit(0);
}