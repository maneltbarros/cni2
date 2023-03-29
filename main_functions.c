/******************************************************************************
 * Last modified: 27-03-2022
 *
 * NAME
 *   main_functions.c
 *
 * DESCRIPTION
 *   Programa com o codigo das funcoes principais para a implementacao do jogo
 *
 * COMMENTS
 *   Todo: 
 * \vertical segfault
 * letras ja no tab horizontal
 * dupla interseção
 ******************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "main_functions.h"

init_info_struct* process_init_input(init_info_struct* info, int argc, char *argv[])
{
    if(argc < 3)
    {
        printf("invalid parameters\n");
        exit(1);
    }
    char* IP = NULL;
    char* TCP = NULL;
    char* regIP = NULL;
    char* regUDP = NULL;
    
    for (int i = 1; i < argc; ++i)
    {
        switch(i)
        {
            case 1:
                IP = argv[i];
                break;
            case 2:
                TCP = argv[i];
                break;
            case 3:
                regIP = argv[i];
                break;
            case 4:
                regUDP = argv[i];
                break;
        }
    }
    info = check_correct_input(IP, TCP, regIP, regUDP, info);

    return info;

}

void free_info(init_info_struct* info)
{
    if (info->IP != NULL)
    {
        free(info->IP);
    }
     if (info->TCP != NULL)
    {
        free(info->TCP);
    }
    if (info->regIP != NULL)
    {
        free(info->regIP);
    }
    if (info->regUDP != NULL)
    {
        free(info->regUDP);
    }
}

void free_node(node_info_struct* node)
{
    
}