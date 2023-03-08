/******************************************************************************
 * Last modified: 27-03-2022
 *
 * NAME
 *   functions.c
 *
 * DESCRIPTION
 *   Program with all the functions for Raiders of all Pyramids
 *
 * COMMENTS
 *   Todo: 
 ******************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "basic_functions.h"

#define MAX_STR 100

init_info_struct* check_correct_input(char* IP, char* TCP, char* regIP, char* regUDP, init_info_struct* info)
{
   if(regUDP == NULL)
   {
        strcpy(info->regUDP, "59000");
   }
   else
   {
        strcpy(info->regUDP, regUDP);
   }

   if(regIP == NULL)
   {
        strcpy(info->regIP, "193.136.138.142");
   }
   else
   {
        strcpy(info->regIP, regIP);
   }

   if(TCP == NULL || IP == NULL)
   {
        return NULL;
   }
   else
   {
        strcpy(info->TCP, TCP);
        strcpy(info->IP, IP);
   }

   return info;
}

id_unique_struct* id_unique(char* str, id_unique_struct* id_struct)
{
     char current_char = '0';
     int flag = 0;
     int k = 0;

     for(int i = 0; current_char != '\0' && i < strlen(str); ++i)
     {
          current_char = str[i];
          if(current_char == '\n')
          {
               if (flag == 3)
               {
                    break;
               }
               flag = 1;
               continue;
          }
          else if(current_char == ' ' && flag != 0)
          {
               if(flag == 1)
               {
                    id_struct->id_unique_bootid[k] = '\0';
               }
               else if(flag == 2)
               {
                    id_struct->id_unique_bootIP[k] = '\0';
               }
               else if(flag == 3)
               {
                    id_struct->id_unique_bootTCP[k] = '\0';
               }
               flag ++;
               k = 0;
               continue;
          }
          if(flag == 1)
          {
               id_struct->id_unique_bootid[k] = current_char;
               k++;
          }
          else if(flag == 2)
          {
               id_struct->id_unique_bootIP[k] = current_char;
               k++;
          }
          else if(flag == 3)
          {
               id_struct->id_unique_bootTCP[k] = current_char;
               k++;
          }
     }

     return id_struct;
}