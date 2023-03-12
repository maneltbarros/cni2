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

#ifndef __udp_functions__
#define __udp_functions__

#include "basic_functions.h"

struct addrinfo *get_udp_server_info(init_info_struct* info);

int open_udp_socket();

void send_message_udp(char* send_str, int fd, struct addrinfo* res);

void recv_message_udp(char* buffer, int fd);


#endif    /* __udp_functions__ */

