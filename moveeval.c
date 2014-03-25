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

#include "claudia.h"
#include "board.h"

int SEE(MOVE *main_capture)
{
    unsigned char dest = DESTMASK(*main_capture);
    unsigned char captured, sq;
    unsigned char attacking_sqs[10];
    unsigned char *less_attack_sq;
    int depth = 0, val = 0;
    MOVE move_hist[20];
    
    MakeMove(main_capture);
    /*Note that color is 0x8(1000) or 0000; ugly.*/
    int attackers = AttackingPieces(dest, (board.white_to_move << 3), attacking_sqs);

    while(attackers){
        less_attack_sq = attacking_sqs;
        for(int i = 0; i < attackers; i++){
            sq = attacking_sqs[i];
            if(board.squares[*less_attack_sq] > board.squares[sq]) {
                less_attack_sq = &attacking_sqs[i];
            }
        }

        move_hist[depth] = (dest << 8) | *less_attack_sq;
        if( (board.squares[*less_attack_sq] == B_PAWN && ROW(dest) == FIRST_ROW)
                || (board.squares[*less_attack_sq] == W_PAWN && ROW(dest) == EIGHT_ROW) ){
            move_hist[depth] |= (B_QUEEN << 16);
        }
        MakeMove(&move_hist[depth]);
        depth++;
        attackers = AttackingPieces(dest, (board.white_to_move << 3), attacking_sqs);
    }

    while(depth--){
        captured = CAPTMASK(move_hist[depth]);
        if(val + Value(captured) > 0) val = -(val + Value(captured));
        else val = 0;
        Takeback(move_hist[depth]);
    }
    captured = CAPTMASK(*main_capture);
    val = val + Value(captured);
    Takeback(*main_capture);

    return val;
}

void SortMoves(MOVE *moves, int nmoves)
{
    int eval[200];
    MOVE hash_move = GetHashMove(board.zobrist_key);
    /*Evaluate every move, store evaluations:*/
    for(int i = 0; i < nmoves; i++){
        eval[i] = EvaluateMove(&moves[i], hash_move);
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
}

int FilterWinning(MOVE *captures, int ncapts)
{
    int good_capts = 0;
    int eval[200];
   
    /*evaluate every move, store evaluations:*/
    for(int i = 0; i < ncapts; i++){
        eval[i] = SEE(&captures[i]);
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

    /*Store the number of 'good' captures:*/
    for(int i = 0; i < ncapts && eval[i] > 0; i++){
        good_capts++;
    }
    return good_capts;
}

int EvaluateMove(MOVE *curr_move, const MOVE hash_move)
{
    /*Compare with hash_move, using only orig, des; curr_move may not have captured or ep info.*/
    if(SQSMASK(*curr_move) == SQSMASK(hash_move)){
        return HASHMOVE_VALUE;        /*search HashMove first.*/
    }
    /*Evaluate captures with SEE:*/
    unsigned char dest = DESTMASK(*curr_move);
    if(board.squares[dest] != EMPTY) return SEE(curr_move);
    else return 0;        /*TODO: evaluate non-captures.*/
}

int Value(unsigned char piece)
{
        switch(piece){

            case W_PAWN: return PAWN_VALUE;
            case B_PAWN: return PAWN_VALUE;

            case W_KNIGHT: return KNIGHT_VALUE;
            case W_BISHOP: return BISHOP_VALUE;
            case W_ROOK: return ROOK_VALUE;

            case B_KNIGHT: return KNIGHT_VALUE;
            case B_BISHOP: return BISHOP_VALUE;
            case B_ROOK: return ROOK_VALUE;
            
            case W_QUEEN: return QUEEN_VALUE;
            case B_QUEEN: return QUEEN_VALUE;
            case W_KING: return KING_VALUE;
            case B_KING: return KING_VALUE;

            default: return 0;
        }
}
