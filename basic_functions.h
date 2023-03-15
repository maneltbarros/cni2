/******************************************************************************
 * Last modified: 27-03-2022
 *
 * NAME
 *   basic_functions.h
 *
 * DESCRIPTION
 *   lista de funcoes auxiliares as funcoes principais
 *
 * COMMENTS
 *
 ******************************************************************************/

#ifndef __basic_functions__
#define __basic_functions__

typedef struct init_info
{
    char* IP;
    char* TCP;
    char* regIP;
    char* regUDP;

}init_info_struct;

typedef struct internal_node_
{
    char id[100];
    char IP[100];
    char TCP[100];
}internal_node;

typedef struct node_info
{
    char* net;
    char* id;
    char* ext;
    char* bck;
    char* ext_IP;
    char* ext_TCP;
    int ext_fd;
    char* bck_IP;
    char* bck_TCP;
    int num_intr;
    internal_node* intr[100];
    int table[100];//inicializar a -1
    char contents[100][100];//fazer malloc e realloc
}node_info_struct;

typedef struct choose_node_struct_
{
    char* chosen_node_bootid;
    char* chosen_node_bootIP;
    char* chosen_node_bootTCP;
}choose_node_struct;

init_info_struct* check_correct_input(char* IP, char* TCP, char* regIP, char* regUDP, init_info_struct* info);

choose_node_struct* choose_node(char* str, choose_node_struct* chosen_node);
char* unique_id(char* buffer, char* id);

init_info_struct* allocate_info();
node_info_struct* allocate_node();
choose_node_struct* init_choose_node_struct(char* id, init_info_struct* info);
internal_node* init_internal_node(char* id, char* IP, char* TCP);

#endif    /* __basic_functions__ */
