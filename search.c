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
#include "engine.h"
#include "hashtable.h"

#define PVLEN 100
#define SPVLEN 600
#define MVLEN 7

static int AssesDraw(const BOARD *board)
{
    if(board->rev_plies[board->ply] >= 50) {
        return DRAW_VALUE;
    }else{
        for(int p = board->ply-board->rev_plies[board->ply]; p < board->ply; p++){
            if(board->zobrist_history[p] == board->zobrist_key){
                return DRAW_VALUE;
            }
        }
    }
    return ERRORVALUE;
}

void IterativeDeep(BOARD *board, CONTROL *control)
{
    MOVE iPV[PVLEN];
    char sPV[SPVLEN];
    char str_mov[MVLEN];
    int eval = 0;
    int alpha = -INFINITE;
    int beta = INFINITE;
    unsigned long long nps = 0;
    MOVE killers[MAXDEPTH] = {0};
    
    for(unsigned int depth = 1; depth <= control->max_depth;){
        unsigned long long curr_time = clock();
        memset(sPV, 0, SPVLEN);
        control->node_count = 0;
        
        eval = AlphaBeta(board, depth, alpha, beta, 1, control, killers);
        if(control->stop){
            break;
        }
        int PVlen = RetrievePV(board, iPV, 2*depth); /*Retrieve longer?*/
        for(int i = 0; i < PVlen; i++){
            MoveToAlgeb(iPV[i], str_mov);
            strcat(sPV, str_mov);
        }
        MoveToAlgeb(control->best_move, str_mov);

        curr_time = (unsigned long long)((clock() - curr_time)/CPMS);
        if(curr_time){
            nps = 1000*(control->node_count/curr_time);
        }
        if(eval > MATE_VALUE/2 && -eval > MATE_VALUE/2){
            printf("info depth %u time %llu nodes %llu nps %llu score cp %i pv %s\n",
                    depth, curr_time, control->node_count, nps, eval, sPV);
        }else if(-eval < MATE_VALUE/2){
            printf("info depth %u time %llu nodes %llu nps %llu score mate %i pv %s\n",
                    depth, curr_time, control->node_count, nps, (PVlen+1)/2, sPV);
        }else if(eval < MATE_VALUE/2){
            printf("info depth %u time %llu nodes %llu nps %llu score mate %i pv %s\n",
                    depth, curr_time, control->node_count, nps, -(PVlen+1)/2, sPV);
        }
        if(eval <= alpha || eval >= beta){
            alpha = -INFINITE;
            beta = INFINITE;
        }else{
            if(3*curr_time > control->wish_time-(clock()-control->init_time)) break;
            alpha = eval - ASP_WINDOW;
            beta = eval + ASP_WINDOW;
            depth++;
        }
    }
    control->stop = 1;
    printf("bestmove %s\n", str_mov);
}

int AlphaBeta(BOARD *board, unsigned int depth, int alpha, int beta, int root, CONTROL *control, MOVE *killers)
{
    int nposs_movs, nlegal = 0;
    MOVE poss_moves[MAXMOVES];
    MOVE best_move = 0;
    char str_mov[MVLEN];
    SQUARE checking_sqs[5];
    int val = ERRORVALUE;
    char hash_flag = HASH_ALPHA;

    if(depth){
        val = GetHashEval(board->zobrist_key, depth, alpha, beta);
        if(val != ERRORVALUE){
            return val;
        }
        if(root == 0 && depth > 2
                && board->piece_material[board->white_to_move] != 0
                && !InCheck(board, 0)){
            MOVE null_mv = NULL_MOVE;
            MakeMove(board, &null_mv);
            val = -AlphaBeta(board, depth-3, -beta, -beta+1, -1, control, killers);
            Takeback(board, null_mv);
            if(val >= beta) return beta;
        }
        nposs_movs = MoveGen(board, poss_moves, 1);
        SortMoves(board, poss_moves, nposs_movs, killers[depth]);
        for(int i = 0; i < nposs_movs; i++){
            MakeMove(board, &poss_moves[i]);
            control->node_count++;
            if(!LeftInCheck(board)){
                if(root== 1 && depth > 6){
                    MoveToAlgeb(poss_moves[i], str_mov);
                    printf("info depth %i hashfull %i currmove %s currmovenumber %i\n",
                        depth, hash_table.full/(hash_table.size/1000), str_mov, i+1);
                }
                nlegal++;
                val = AssesDraw(board);
                
                if(val) {
                    if(best_move){
                        val = -AlphaBeta(board, depth - 1, -alpha-1, -alpha, 0, control, killers);
                        if(val > alpha && val < beta){
                            val = -AlphaBeta(board, depth - 1, -beta, -alpha, 0, control, killers);
                        }
                    }else val = -AlphaBeta(board, depth - 1, -beta, -alpha, 0, control, killers);
                }
                Takeback(board, poss_moves[i]);

                if(clock() - control->init_time > control->max_time*CPMS*0.95){
                    control->stop = 1;
                }
                if(control->stop) return alpha;
                if(val >= beta){
                    UpdateTable(board->zobrist_key, val, poss_moves[i], depth, HASH_BETA);
                    MOVE hash_move = GetHashMove(board->zobrist_key);
                    if(SQSMASK(poss_moves[i]) != SQSMASK(hash_move)
                          && CAPTMASK(poss_moves[i]) == 0){
                        killers[depth] = poss_moves[i];
                    }
                    return beta;
                }
                if(val > alpha){
                    alpha = val;
                    hash_flag = HASH_EXACT;
                    best_move = poss_moves[i];
                    if(root == 1) control->best_move = best_move;
                }
                if(root == 1 && ((clock() - control->init_time) > control->wish_time*CPMS)){
                /*don't search anymore after current move; TODO: continue if fails low?*/
                    control->max_depth = depth;
                    return alpha;
                }
            }else Takeback(board, poss_moves[i]);
        }
        if(nlegal == 0){
            if(InCheck(board, 0)){
/*UpdateTable(board->zobrist_key, MATE_VALUE+board->ply, 0, depth, HASH_EXACT, hash_table.entries);*/
                return MATE_VALUE;
            }else{
/*UpdateTable(board->zobrist_key, DRAW_VALUE, 0, depth, HASH_EXACT, hash_table.entries);*/
                return DRAW_VALUE;    /*Stalemate*/
            }
        }else UpdateTable(board->zobrist_key, alpha, best_move, depth, hash_flag);
        
    }else if(InCheck(board, 0)){
        alpha = AlphaBeta(board, 1, alpha, beta, 0, control, killers);
    }else{
        alpha = Quiescent(board, alpha, beta, control);
    }
    return alpha;
}

int Quiescent(BOARD* board, int alpha, int beta, CONTROL *control)
{
    int nposs_movs, nlegal = 0;
    int val;
    MOVE poss_moves[MAXMOVES];

    val = LazyEval(board);
    if(val-LAZYBETA >= beta) return beta;
    if(val+LAZYALPHA < alpha) return alpha;

    val = StaticEval(board);
    UpdateTable(board->zobrist_key, val, 0, 0, HASH_EXACT);

    if (val >= beta) return beta;
    if (val > alpha) alpha = val;
    
    nposs_movs = CaptureGen(board, poss_moves);
    nposs_movs = FilterWinning(board, poss_moves, nposs_movs);

    for(int i = 0; i < nposs_movs; i++){
        control->node_count++;
        MakeMove(board, &poss_moves[i]);
        if(!LeftInCheck(board)){
            nlegal++;
            val = -Quiescent(board, -beta, -alpha, control);
            Takeback(board, poss_moves[i]);
            if(val >= beta){
                return beta;
            }
            if(val > alpha){
                alpha = val;
            }
        }else Takeback(board, poss_moves[i]);
    }
    return alpha;
}

/*This only works because the replacement scheme ensures shallow PV is not overwritten,
and may fail if the hash table is full.*/
int RetrievePV(BOARD *board, MOVE *PV, unsigned int depth)
{
    unsigned int PVlen = 0;
    MOVE mov = GetHashMove(board->zobrist_key);
    while(mov && PVlen <= depth && IsLegal(board, &mov)){
        PV[PVlen] = mov;
        PVlen++;
        MakeMove(board, &mov);
        mov = GetHashMove(board->zobrist_key);
    }
    for(int i = PVlen; i > 0; i--){
        Takeback(board, PV[i-1]);
    }
    return PVlen;
}
