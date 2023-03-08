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

#ifndef __ui_functions__
#define __ui_functions__

#include "basic_functions.h"
#include "main_functions.h"

char join(char* net, char* id, struct addrinfo* res, init_info_struct* info, node_info_struct* node);
char djoin(char* net, char* id, char* bootid, char* bootIP, char* bootTCP, struct addrinfo* res, init_info_struct* info, node_info_struct* node);
char create(char* name);
char delete_fctn(char* name);
char get_fctn(char* dest, char* name);
char show_topology();
char show_names();
char show_routing();
char leave(struct addrinfo* res, init_info_struct* info, node_info_struct* node);
void exit_fctn(char* str1, char* str2, char* str3, char* str4, char* str5, char* str6, struct addrinfo* res, init_info_struct* info);


#endif    /* __ui_functions__ */