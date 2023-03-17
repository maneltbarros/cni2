/******************************************************************************
 * Last modified: 27-03-2022
 *
 * NAME
 *   main_functions.h
 *
 * DESCRIPTION
 *   lista de funcoes principais para a implementacao do jogo
 *
 * COMMENTS
 *
 ******************************************************************************/

#ifndef __main_functions__
#define __main_functions__

#include "basic_functions.h"

init_info_struct* process_init_input(init_info_struct* info, int argc, char *argv[]);
void free_info(init_info_struct* info);
void free_node(node_info_struct* node);

#endif    /* __main_functions__ */
