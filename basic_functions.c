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

choose_node_struct* choose_node(char* str, choose_node_struct* chosen_node)
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
                    chosen_node->chosen_node_bootid[k] = '\0';
               }
               else if(flag == 2)
               {
                    chosen_node->chosen_node_bootIP[k] = '\0';
               }
               else if(flag == 3)
               {
                    chosen_node->chosen_node_bootTCP[k] = '\0';
               }
               flag ++;
               k = 0;
               continue;
          }
          if(flag == 1)
          {
               chosen_node->chosen_node_bootid[k] = current_char;
               k++;
          }
          else if(flag == 2)
          {
               chosen_node->chosen_node_bootIP[k] = current_char;
               k++;
          }
          else if(flag == 3)
          {
               chosen_node->chosen_node_bootTCP[k] = current_char;
               k++;
          }
     }

     return chosen_node;
}

char* unique_id(char* buffer, char* id)
{
     char current_char = '0';
     int id_in_this_line_done = 0;
     int id_chars_read = 0;
     int first_line_read = 0;
     int available_ids[100];
     char current_id[3];

     for(int i = 0; i < 100; ++i)
     {
          available_ids[i] = 1;
     }

     for(int i = 0; current_char != '\0' && i < strlen(buffer); ++i)
     {
          current_char = buffer[i];
          if(current_char == '\n') //next line
          {
               id_in_this_line_done = 0;
               id_chars_read = 0;
               first_line_read = 1;
          }
          else if(first_line_read)
          {
               if(id_in_this_line_done != 1) //if ==1 then wait for next line
               {
                    if(id_chars_read == 0)
                    {
                         current_id[0] = current_char;
                         id_chars_read = 1;
                    }
                    else      //id only has 2 chars so stop reading id here
                    {
                         current_id[1] = current_char;
                         current_id[2] = '\0';
                         available_ids[atoi(current_id)] = 0;    //id no longer available
                         id_in_this_line_done = 1;
                    }
               }
          }
     }

     if(available_ids[atoi(id)] == 1)
     {
          return id;          //id available
     }
     else
     {
          for(int i = 99; i > 0; --i)   //search for first available id, starting from last (more likely to be available)
          {
               if(available_ids[i] == 1)
               {
                    //maybe try this:   sprintf(id, "%s", i);
                    id[0] = i/10 + 48;
                    id[1] = i%10 + 48;
                    id[2] = '\0';
                    return id;
               }
          }
          strcpy(id, "-1");
     }
     return id;
}

int is_valid_id(char* str)
{
     if(strlen(str) != 2) return 0;

     for(int i = 0; i < strlen(str); ++i)
     {
          if(str[i] > '9' || str[i] < '0') return 0;
     }
     return 1;
}

int is_valid_net(char* str)
{
     if(strlen(str) != 3) return 0;

     for(int i = 0; i < strlen(str); ++i)
     {
          if(str[i] > '9' || str[i] < '0') return 0;
     }
     return 1;
}

init_info_struct* allocate_info()
{
    init_info_struct* info = (init_info_struct*)malloc(sizeof(init_info_struct));
    info->IP = (char*)malloc(MAX_STR);
    info->TCP = (char*)malloc(MAX_STR);
    info->regIP = (char*)malloc(MAX_STR);
    info->regUDP = (char*)malloc(MAX_STR);
    return info;
}

node_info_struct* allocate_node()
{
    node_info_struct* node = (node_info_struct*)malloc(sizeof(node_info_struct));
    node->net = (char*)malloc(5);
    node->id = (char*)malloc(5);
    node->ext = (char*)malloc(5);
    node->ext_IP = (char*)malloc(MAX_STR);
    node->ext_TCP = (char*)malloc(MAX_STR);
    node->bck_IP = (char*)malloc(MAX_STR);
    node->bck_TCP = (char*)malloc(MAX_STR);
    node->bck = (char*)malloc(5);
    node->num_intr = 0;
    node->num_content = 0;
    for(int i = 0; i < 100;++i)
    {
          node->table[i] = -1;
    }
    for(int i = 0; i < 100;++i)
    {
          memset(node->contents[i], 0, sizeof(node->contents[i]));
    }
    return node;
}

choose_node_struct* init_choose_node_struct(char* id, init_info_struct* info)
{
     choose_node_struct* chosen_node = (choose_node_struct*)malloc(sizeof(choose_node_struct));
     memset(chosen_node->chosen_node_bootid, 0, sizeof(chosen_node->chosen_node_bootid));
     memset(chosen_node->chosen_node_bootIP, 0, sizeof(chosen_node->chosen_node_bootIP));
     memset(chosen_node->chosen_node_bootTCP, 0, sizeof(chosen_node->chosen_node_bootTCP ));

     strcpy(chosen_node->chosen_node_bootid, id);
     strcpy(chosen_node->chosen_node_bootIP, info->IP);
     strcpy(chosen_node->chosen_node_bootTCP, info->TCP);

     return chosen_node;
}

internal_node* init_internal_node(char* id, char* IP, char* TCP)
{
     internal_node* ret = (internal_node*)malloc(sizeof(internal_node ));
     strcpy(ret->id, id);
     strcpy(ret->IP, IP);
     strcpy(ret->TCP, TCP);
     return ret;
}

void reset_node(node_info_struct* node)
{
     strcpy(node->net, "0");
     strcpy(node->id, "0");
     strcpy(node->ext, "0");
     strcpy(node->ext_IP, "0");
     strcpy(node->ext_TCP, "0");
     strcpy(node->bck, "0");
     strcpy(node->bck_IP, "0");
     strcpy(node->bck_TCP, "0");
     node->ext_fd = -1;
     for (int i = 0; i < node->num_intr; i++)
     {
          free(node->intr[i]);
     }
     
     node->num_intr = 0;
     node->num_content = 0;
     for (int i = 0; i < 100; i++)
     {
          node->table[i] = -1;
     }
     
}
