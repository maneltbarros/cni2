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

#ifndef __tcp_functions__
#define __tcp_functions__

#include "basic_functions.h"

struct addrinfo *get_tcp_server_info(char* bootIP, char* boot_TCP);

int open_tcp_socket();

void tcp_connect(int fd_TCP, struct addrinfo* res_TCP);

void send_tcp(char* send_str, int fd_TCP, char* buffer);

int open_tcp_server(char* IP, char* TCP);

int accept_connection(int fd);

void receive_and_send_tcp(int newfd, int* fdes, init_info_struct* info, node_info_struct* node, fd_set* inputs);

void fix_topology(int id_do_gajo, node_info_struct* node, init_info_struct* info, int* fdes, fd_set* inputs);

char* NEW(char* id, char* IP, char* TCP, node_info_struct* node, char* return_value, int* fdes, int newfd);
char* EXTERN(char* bck, char* IP, char* TCP, node_info_struct* node, char* return_value);
char* QUERY(char* dest, char* orig, char* name, node_info_struct* node, char* return_value, int* fdes, int newfd);
char* CONTENT(char* dest, char* orig, char* name, node_info_struct* node, char* return_value, int* fdes, int newfd);
char* N_CONTENT(char* dest, char* orig, char* name, node_info_struct* node, char* return_value, int* fdes, int newfd);
void update_intr(int* fdes, int newfd, node_info_struct* node, char* orig);
void update_ext(int newfd, node_info_struct* node, char* orig);


#endif    /* __tcp_functions__ */

