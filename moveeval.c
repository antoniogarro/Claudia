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

#include "board.h"
#include "engine.h"

static int SEE(BOARD *board, MOVE *main_capture)
{
    SQUARE dest = DESTMASK(*main_capture);
    SQUARE captured, sq;
    SQUARE attacking_sqs[10];
    SQUARE less_attack_sq;
    int depth = 0, val = 0;
    MOVE move_hist[20];
    
    MakeMove(board, main_capture);
    
    int attackers = AttackingPieces(board, dest, board->white_to_move, attacking_sqs);

    while(attackers){
        less_attack_sq = attacking_sqs[0];
        for(int i = 0; i < attackers; i++){
            sq = attacking_sqs[i];
            if(board->squares[less_attack_sq] > board->squares[sq]) {
                less_attack_sq = attacking_sqs[i];
            }
        }
        
        if( (board->squares[less_attack_sq] == B_PAWN && ROW(dest) == FIRST_ROW)
                || (board->squares[less_attack_sq] == W_PAWN && ROW(dest) == EIGHT_ROW) ){
            move_hist[depth] = Move(B_QUEEN, dest, less_attack_sq);
        }
        else move_hist[depth] = Move(0, dest, less_attack_sq);
        
        MakeMove(board, &move_hist[depth++]);
        attackers = AttackingPieces(board, dest, board->white_to_move, attacking_sqs);
    }

    while(depth--){
        captured = CAPTMASK(move_hist[depth]);
        if(val + Value(captured) > 0) val = -(val + Value(captured));
        else val = 0;
        Takeback(board, move_hist[depth]);
    }
    captured = CAPTMASK(*main_capture);
    val = val + Value(captured);
    Takeback(board, *main_capture);

    return val;
}

static int QuietMoveEval(BOARD *board, MOVE *move, const MOVE killers[])
{
    if(SQSMASK(*move) == SQSMASK(killers[0]) || SQSMASK(*move) == SQSMASK(killers[1])){
        return KILLER_VALUE;
    }else if(PROMMASK(*move)){
        return PROMOTION_VALUE;
    }else{
        return 0;
    }
}

static int EvaluateMove(BOARD *board, MOVE *curr_move, const MOVE hash_move, const MOVE killers[])
{
    /*Compare with hash_move, using only orig, des; curr_move may not have captured or ep info.*/
    if(SQSMASK(*curr_move) == SQSMASK(hash_move)){
        return HASHMOVE_VALUE;        /*search HashMove first.*/
    }
    /*Evaluate captures with SEE:*/
    SQUARE dest = DESTMASK(*curr_move);
    if(board->squares[dest] != EMPTY){
        return SEE(board, curr_move);
    }else{
        return QuietMoveEval(board, curr_move, killers);
    }
}

int SortMoves(BOARD *board, MOVE *moves, int nmoves, MOVE killers[])
{
    int eval[MAXMOVES], good = 0;
    MOVE hash_move = GetHashMove(&hash_table, board->zobrist_key);
    /*Evaluate every move, store evaluations:*/
    for(int i = 0; i < nmoves; i++){
        eval[i] = EvaluateMove(board, &moves[i], hash_move, killers);
	if(eval[i] > 0) good++;
    }
    /*Order according to that evaluation: insertion sort*/
    for(int i = 1; i < nmoves; i++){
        for(int j = i; j > 0 && (eval[j-1] < eval[j]); j--){            
            MOVE tmp = moves[j-1];
            moves[j-1] = moves[j];
            moves[j] = tmp;
            
            int ev = eval[j-1];
            eval[j-1] = eval[j];
            eval[j] = ev;
        }
    }
    return good;
}

int FilterWinning(BOARD *board, MOVE *captures, int ncapts)
{
    int good_capts = 0;
    int eval[MAXMOVES];
   
    /*evaluate every move, store evaluations:*/
    for(int i = 0; i < ncapts; i++){
        eval[i] = SEE(board, &captures[i]);
	if(eval[i] > 0) good_capts++;
    }
    for(int i = 1; i < ncapts; i++){
        for(int j = i; j > 0 && (eval[j-1] < eval[j]); j--){
            MOVE tmp = captures[j-1];
            captures[j-1] = captures[j];
            captures[j] = tmp;
            
            int ev = eval[j-1];
            eval[j-1] = eval[j];
            eval[j] = ev;
        }
    }
    return good_capts;
}
