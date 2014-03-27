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

#ifndef ENGINEH
#define ENGINEH

#include <time.h>
#include "board.h"

static const clock_t CPMS = CLOCKS_PER_SEC/1000;

typedef struct CONTROL {
    int wtime, btime, wtime_inc, btime_inc;
    clock_t init_time, wish_time, max_time;
    unsigned int max_depth;
    unsigned long long node_count;
    char uci, stop;
    MOVE best_move;
} CONTROL;

typedef struct ENGINE_STATE{
    BOARD *board;
    CONTROL *control;
} ENGINE_STATE;

/*UCI commands*/
typedef int CMDFN(char*, ENGINE_STATE*);

struct UCI_COMMAND{
    char *name;
    CMDFN *cmd;
};

void IterativeDeep(BOARD*, CONTROL*);
int AlphaBeta(BOARD*, unsigned int, int, int, int, CONTROL*, MOVE*);
int Quiescent(BOARD*, int, int, CONTROL*);
int RetrievePV(BOARD*, MOVE*, unsigned int);

int ParseCommand(const char*);
int Command(const char*, ENGINE_STATE*);

int PolyglotChooseMove(KEY key);

static void think(void *pparams)
{   
    BOARD *board = ((ENGINE_STATE*)pparams)->board;
    CONTROL *control = ((ENGINE_STATE*)pparams)->control;
    
    if(PolyglotChooseMove(PolyglotKey(board))){
        control->stop = 1;
        return;
    }
    IterativeDeep(board, control);
    control->stop = 1;
    return;
}

#endif
