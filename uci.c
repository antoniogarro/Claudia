/***************************************************************************************
* Copyright (c) 2014, Antonio Garro.                                                   *
* All rights reserved                                                                  *
*                                                                                      *
* Redistribution and use in source and binary forms, with or without modification, are *
* permitted provided that the following conditions are met:                            *
*                                                                                      *
* 1. Redistributions of source code must retain the above copyright notice, this list  *
* of conditions and the following disclaimer.                                          *
*                                                                                      *
* 2. Redistributions in binary form must reproduce the above copyright notice, this    *
* list of conditions and the following disclaimer in the documentation and/or other    *
* materials provided with the distribution.                                            *
*                                                                                      *
* 3. Neither the name of the copyright holder nor the names of its contributors may be *
* used to endorse or promote products derived from this software without specific      *
* prior written permission.                                                            *
*                                                                                      * 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"          *
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE            *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE           *
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE            *
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL    *
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR           *
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER           *
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR     *
* TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF        *
* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                    *
***************************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef __linux__
#include <pthread.h>
#elif _WIN32
#include <process.h>
#endif
#include "engine.h"
#include "claudia.h"
#include "hashtable.h"


const struct UCI_COMMAND uci_commands[] = {{"INV", &make_move},
                                            {"quit", &quit},
                                            {"go", &go},
                                            {"stop", &stop},
                                            {"perft", &perft},
                                            {"position", &position},
                                            {"uci", &uci},
                                            {"isready", &isready},
                                            {"setoption", &setoption}};
                                         
int ParseCommand(const char *command)
{    
    for(int i = 0; i < sizeof(uci_commands)/sizeof(struct UCI_COMMAND); i++){
        if(!strcmp(command, uci_commands[i].name)) return i;
    }
    return 0;
}

int Command(const char *input)
{
    char input_copy[2048], cmd_str[20];
    strcpy(input_copy, input);
    sscanf(input, "%s", cmd_str);
    int cmd = ParseCommand(cmd_str);
    return uci_commands[cmd].cmd(input_copy);
}

void ManageTimes(int nmoves)
{
    if(board.white_to_move){
        control.wish_time = control.wtime/nmoves + control.wtime_inc;
        control.max_time = control.wtime;
    }
    else{
        control.wish_time = control.btime/nmoves + control.btime_inc;
        control.max_time = control.btime;
    }
}

void ResetTimes()
{
    control.wtime = 0; control.btime = 0;
    control.wtime_inc = 0; control.btime_inc = 0;
}

int make_move(char *input)
{
    char *str_param = strtok(input, " \n\t");
    if(str_param){
        move curr_move = AlgebToMove(str_param);
        if(IsLegal(&curr_move)) MakeMove(&curr_move);
    }
    if(!control.uci) PrintBoard();
    return 1;
}

int quit(char *input)
{
    control.stop = 1;
    return 0;
}

int go(char *input)
{    
    control.max_depth = 70;
    control.max_time = INFINITE;
    control.wish_time = INFINITE;
    control.stop = 1;
    ResetTimes();
    int movestogo = 30;
    char manage_times = 1;
    char *str_param = strtok(input, " \n\t");
    for(str_param = strtok(NULL, " \n\t"); str_param; str_param = strtok(NULL, " \n\t")){
        if(!strcmp("depth",  str_param)){
            str_param = strtok(NULL, " \n\t");
            if(str_param){
                control.max_depth = atoi(str_param);
                manage_times = 0;
            }
            break;
        }else if(!strcmp("time",  str_param)){
            str_param = strtok(NULL, " \n\t");
            if(str_param){
                control.wish_time = atol(str_param);
                control.max_time = atol(str_param);
                manage_times = 0;
            }
            break;
        }else if(!strcmp("infinite",  str_param)){
            manage_times = 0;
            break;
        }else if(!strcmp("btime",  str_param)){
            str_param = strtok(NULL, " \n\t");
            if(str_param){
                control.btime = atoi(str_param);
            }
        }else if(!strcmp("wtime",  str_param)){
            str_param = strtok(NULL, " \n\t");
            if(str_param){
                control.wtime = atoi(str_param);
            }
        }else if(!strcmp("binc",  str_param)){
            str_param = strtok(NULL, " \n\t");
            if(str_param){
                control.btime_inc = atoi(str_param);
            }
        }else if(!strcmp("winc",  str_param)){
            str_param = strtok(NULL, " \n\t");
            if(str_param){
                control.wtime_inc = atoi(str_param);
            }
        }else if(!strcmp("movestogo",  str_param)){
            str_param = strtok(NULL, " \n\t");
            if(str_param){
                movestogo = atoi(str_param);
            }
        }else break;
    }

    if(control.stop){
        if(manage_times){
            ManageTimes(movestogo);
        }
        control.stop = 0;
        control.init_time = clock();
#ifdef __linux__
        pthread_attr_t tattr;
        pthread_attr_init(&tattr);
        pthread_attr_setdetachstate(&tattr,PTHREAD_CREATE_DETACHED);

        pthread_t thread_id;
        pthread_create(&thread_id, &tattr, (void *)(think), 0);
#elif _WIN32
        _beginthread(think, 0, NULL);
#endif
    }
    return 1;
}

int stop(char *input)
{
    control.stop = 1;
    return 1;
}

int perft(char *input)
{
    int depth = 0;
    sscanf(input, "perft %i", &depth);
    control.init_time = clock();
    printf("Depth: %i Moves: %i\n", depth, Perft(depth));
    printf("Time elapsed: %llu ms\n", (unsigned long long)(clock() - control.init_time));
    return 1;
}

int position(char *input)
{
    strtok(input, " \n\t");
    char *str_param = strtok(NULL, " \n\t");
    if(!strcmp(str_param, "startpos")) ReadFEN(STARTPOS);
    else if(!strcmp(str_param, "fen")){
        /*Don't split chain at spaces; cut at 'm' for 'moves' or at '\n'.*/
        str_param = strtok(NULL, "m\n\t"); 
        if(str_param){
                ReadFEN(str_param);
        }
    } else return 0;
    
    str_param = strtok(NULL, " \n\t");  /*e.g. str_param == "moves"*/
    str_param = strtok(NULL, " \n\t");  /*e.g. str_param == "e2e4"*/
    while(str_param){
        move curr_move = AlgebToMove(str_param);
        if(IsLegal(&curr_move)) MakeMove(&curr_move);
        else return 0;
        str_param = strtok(NULL, " \n\t");
    }
    return 1;
}

int uci(char *input)
{
    control.uci = 1;
    printf("id name %s v. %s\n", NAME, VERSION);
    printf("id author Antonio Garro\n");
    printf("option name Hash type spin default 32 min 32 max 2048\n");
    printf("uciok\n");
    return 1;
}

int isready(char *input)
{
    printf("readyok\n");
    return 1;
}

int setoption(char *input)
{
    int val = 0;
    sscanf(input, "setoption name Hash value %i", &val);
    DeleteTable();
    if(AllocTable(val) == 0) return 0;
    ClearHashTable();
    return 1;
}
