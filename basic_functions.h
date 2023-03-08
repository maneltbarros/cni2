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

typedef struct node_info
{
    char* net;
    char* id;
    char* ext;
    char* bck;
    char* ext_IP;
    char* ext_TCP;
    char** intr;
    char*** table;
}node_info_struct;

typedef struct id_unique_struct_
{
    char* id_unique_bootid;
    char* id_unique_bootIP;
    char* id_unique_bootTCP;
}id_unique_struct;

init_info_struct* check_correct_input(char* IP, char* TCP, char* regIP, char* regUDP, init_info_struct* info);

id_unique_struct* id_unique(char* str, id_unique_struct* id_struct);

#endif    /* __basic_functions__ */
