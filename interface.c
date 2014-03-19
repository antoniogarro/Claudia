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


int Command(char *input)
{
    char manage_times;
    int command, params;
    char *str_command = strtok(input, " \n\t");
    char *str_param;
    command = ParseInput(str_command);

    move curr_move;

    switch(command){
        case COM_PERFT:
            str_param = strtok(NULL, " \n\t");
            params = atoi(str_param);
            control.init_time = clock();
            printf("Depth: %i Moves: %i\n", params, Perft(params));
            printf("Time elapsed: %llu ms\n", (unsigned long long)(clock() - control.init_time));
            break;

        case COM_GO:
            control.max_depth = 70;
            control.max_time = INFINITE;
            control.wish_time = INFINITE;
            control.stop = 1;
            ResetTimes();
            manage_times = 1;
            do{
                str_param = strtok(NULL, " \n\t");
                params = ParseGoParams(str_param);
                switch(params){
                    case GO_DEPTH:
                        str_param = strtok(NULL, " \n\t");
                        control.max_depth = atoi(str_param);
                        manage_times = 0;
                        break;
                    case GO_TIME:
                        str_param = strtok(NULL, " \n\t");
                        control.wish_time = atol(str_param);
                        control.max_time = atol(str_param);
                        manage_times = 0;
                        break;
                    case GO_INFINITE:
                        manage_times = 0;
                        break;
                    case B_TIME:
                        str_param = strtok(NULL, " \n\t");
                        control.btime = atoi(str_param);
                        break;
                    case W_TIME:
                        str_param = strtok(NULL, " \n\t");
                        control.wtime = atoi(str_param);
                        break;
                    case B_INC:
                        str_param = strtok(NULL, " \n\t");
                        control.btime_inc = atoi(str_param);
                        break;
                    case W_INC:
                        str_param = strtok(NULL, " \n\t");
                        control.wtime_inc = atoi(str_param);
                        break;
                    default:
                        break;
                }
            }while(str_param);

            if(control.stop){
                if(manage_times){
                    ManageTimes();
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
            break;

        case COM_STOP:
            control.stop = 1;
            break;

        case COM_MOVE:
            str_param = strtok(str_command, " \n\t");
            curr_move = AlgebToMove(str_param);
            if(IsLegal(&curr_move))
                MakeMove(&curr_move);
            if(!control.uci) PrintBoard();
            break;

        case COM_POSITION:
            str_param = strtok(NULL, " \n\t");
            if(!strcmp(str_param, "startpos")) ReadFEN(STARTPOS);
            else{
                if(!strcmp(str_param, "fen")){
                    /*Shouldn't cut chain at spaces; cut at 'm' for 'moves' or at '\n'.*/
                    str_param = strtok(NULL, "m\n\t"); 
                    ReadFEN(str_param);
                }
            }
            str_param = strtok(NULL, " \n\t");    /*e.g. str_param == "moves"*/
            /*if(!str_param || strcmp(str_param, "oves")) break;*/
            /*actually, it may be str_param == "oves", as strtok removes the 'm'; ugly.*/
            str_param = strtok(NULL, " \n\t");    /*e.g. str_param == "e2e4", e.g.*/
            while(str_param){
                    curr_move = AlgebToMove(str_param);
                    MakeMove(&curr_move);
                    str_param = strtok(NULL, " \n\t");
            }
            break;

        case COM_UCI:
            control.uci = 1;
            printf("id name %s v. %s\n", NAME, VERSION);
            printf("id author Antonio Garro\n");
            printf("option name Hash type spin default 32 min 32 max 2048\n");
            printf("uciok\n");
            break;

        case COM_IS_READY:
            printf("readyok\n");
            break;

        case COM_QUIT:
            control.stop = 1;
            return 0;
        
        case COM_SETOPTION:
            str_param = strtok(NULL, " \n\t");
            if(strcmp(str_param, "name")) break;
        
            str_param = strtok(NULL, " \n\t");
            if(strcmp(str_param, "Hash")) break;
        
            str_param = strtok(NULL, " \n\t");
            if(strcmp(str_param, "value")) break;
        
            str_param = strtok(NULL, " \n\t");
            params = atoi(str_param);
            
            DeleteTable();
            if(AllocTable(params) == 0) break;
            ClearHashTable();
            break;

        default:
            break;
    }
    return 1;
}
