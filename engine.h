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

#define NAME  "Claudia"
#define VERSION "0.2"


#ifndef VALUES
#define VALUES

#define ERRORVALUE -1000000001
#define INFINITE 10000000
#define DRAW_VALUE 0
#define MATE_VALUE -100000
#define HASHMOVE_VALUE 100000
#define KILLER_VALUE 50

#define PAWN_VALUE 100
#define KNIGHT_VALUE 300
#define BISHOP_VALUE 320
#define ROOK_VALUE 500
#define QUEEN_VALUE 900

#define STARTPAWNS 1600
#define STARTMATERIAL 6280

static const int piece_values[] = {
    0, 0,
    PAWN_VALUE, PAWN_VALUE,
    KNIGHT_VALUE, KNIGHT_VALUE,
    BISHOP_VALUE, BISHOP_VALUE,
    ROOK_VALUE, ROOK_VALUE,
    QUEEN_VALUE, QUEEN_VALUE,
    -MATE_VALUE, -MATE_VALUE
};

#define N_MOBILITY 5
#define B_MOBILITY 4
#define R_MOBILITY 1
#define Q_MOBILITY 1
#define K_MOBILITY 0
static const int mobility_bonus[] = {
     0, 0,
    -1, 1,
    -N_MOBILITY, N_MOBILITY,
    -B_MOBILITY, B_MOBILITY,
    -R_MOBILITY, R_MOBILITY,
    -Q_MOBILITY, Q_MOBILITY,
    -K_MOBILITY, K_MOBILITY
};

#define CASTLE_BONUS 60
#define CASTLE_RIGHT_BONUS 20

#define DOUBLED_PAWN_BONUS 10
#define ISOLATED_PAWN_BONUS 5
#define PAWN_PUSH_BONUS 0.1
#define PASSED_PAWN_BONUS 20

#endif

#ifndef SEARCH
#define SEARCH
#define ASP_WINDOW 50
#define LAZYBETA 400
#define LAZYALPHA 1000
#define MAXMOVES 250
#define MAXDEPTH 70
#endif

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
