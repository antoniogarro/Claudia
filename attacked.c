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
#include "claudia.h"

char IsAttacked(unsigned char square, unsigned char attacking_color)
{
    unsigned char attacking_sq = 0;
    if(attacking_color){
        for(int i = 0; w_pawn_capture[i]; i++){
            attacking_sq = square - w_pawn_capture[i];
            if(IN_BOARD(attacking_sq) && board.squares[attacking_sq] == W_PAWN){
                return 1;
            }
        }
    }else{
        for(int i = 0; b_pawn_capture[i]; i++){
            attacking_sq = square - b_pawn_capture[i];
            if(IN_BOARD(attacking_sq) && board.squares[attacking_sq] == B_PAWN){
                return 1;
            }
        }
    }

    for(int i = 0; knight_delta[i]; i++){
        attacking_sq = square + knight_delta[i];
        if(IN_BOARD(attacking_sq)){
            if(board.squares[attacking_sq] == BLACK_TO_COLOR(B_KNIGHT, attacking_color)){
                return 1;
            }
        }
    }

    for(int i = 0; bishop_delta[i]; i++){
        for(attacking_sq = square + bishop_delta[i]; IN_BOARD(attacking_sq); attacking_sq += bishop_delta[i]){
            if(board.squares[attacking_sq] == EMPTY){
                continue;
            }else if(board.squares[attacking_sq] == BLACK_TO_COLOR(B_BISHOP, attacking_color)) {
                return 1;
            }else if(board.squares[attacking_sq] == BLACK_TO_COLOR(B_QUEEN, attacking_color)){
                return 1;        /*attacked square, hence exit.*/
            }else break;
        }
    }

    for(int i = 0; rook_delta[i]; i++){
        for(attacking_sq = square + rook_delta[i]; IN_BOARD(attacking_sq); attacking_sq += rook_delta[i]){            
            if(board.squares[attacking_sq] == EMPTY){
                continue;
            }else if(board.squares[attacking_sq] == BLACK_TO_COLOR(B_ROOK, attacking_color)){
                return 1;
            }else if(board.squares[attacking_sq] == BLACK_TO_COLOR(B_QUEEN, attacking_color)){
                return 1;
            }else break;
        }
    }
    for(int i = 0; king_delta[i]; i++){
        attacking_sq = square + king_delta[i];
        if(IN_BOARD(attacking_sq)){
            if(board.squares[attacking_sq] == BLACK_TO_COLOR(B_KING, attacking_color)){
                return 1;
            }
        }
    }
    return 0;
}

int AttackingPieces(unsigned char square, unsigned char attacking_color, unsigned char * attacking_sqs)
{
    int attackers = 0;
    unsigned char attacking_sq = 0;
    if(attacking_color){
        for(int i = 0; w_pawn_capture[i]; i++){
            attacking_sq = square - w_pawn_capture[i];
            /*There should be no PAWN outside the board, but happens.*/
            if(IN_BOARD(attacking_sq) && board.squares[attacking_sq] == W_PAWN){
                attacking_sqs[attackers] = attacking_sq;
                attackers++;
            }
        }
    }else{
        for(int i = 0; b_pawn_capture[i]; i++){
            attacking_sq = square - b_pawn_capture[i];
            if(IN_BOARD(attacking_sq) && board.squares[attacking_sq] == B_PAWN){
                attacking_sqs[attackers] = attacking_sq;
                attackers++;
            }
        }
    }

    for(int i = 0; knight_delta[i]; i++){
        attacking_sq = square + knight_delta[i];
        if(IN_BOARD(attacking_sq)){
            if(board.squares[attacking_sq] == BLACK_TO_COLOR(B_KNIGHT, attacking_color)){
                attacking_sqs[attackers] = attacking_sq;
                attackers++;
            }
        }
    }

    for(int i = 0; bishop_delta[i]; i++){
        for(attacking_sq = square + bishop_delta[i]; IN_BOARD(attacking_sq); attacking_sq += bishop_delta[i]){
            if(board.squares[attacking_sq] == EMPTY){
                continue;
            }else if(board.squares[attacking_sq] == BLACK_TO_COLOR(B_BISHOP, attacking_color)) {
                attacking_sqs[attackers] = attacking_sq;
                attackers++;
                break;
            }else if(board.squares[attacking_sq] == BLACK_TO_COLOR(B_QUEEN, attacking_color)){
                attacking_sqs[attackers] = attacking_sq;
                attackers++;
                break;
            }else break; /*Piece other than bishop/queen, or different color, stop sliding*/
        }
    }

    for(int i = 0; rook_delta[i]; i++){
        for(attacking_sq = square + rook_delta[i]; IN_BOARD(attacking_sq); attacking_sq += rook_delta[i]){
            if(board.squares[attacking_sq] == EMPTY){
                continue;
            }else if(board.squares[attacking_sq] == BLACK_TO_COLOR(B_ROOK, attacking_color)){
                attacking_sqs[attackers] = attacking_sq;
                attackers++;
                break;
            }else if(board.squares[attacking_sq] == BLACK_TO_COLOR(B_QUEEN, attacking_color)){
                attacking_sqs[attackers] = attacking_sq;
                attackers++;
                break;
            }else break;
        }
    }
    for(int i = 0; king_delta[i]; i++){
        attacking_sq = square + king_delta[i];
        if(IN_BOARD(attacking_sq)){
            if(board.squares[attacking_sq] == BLACK_TO_COLOR(B_KING, attacking_color)){
                attacking_sqs[attackers] = attacking_sq;
                attackers++;
                break;
            }
        }
    }
    return attackers;
}
