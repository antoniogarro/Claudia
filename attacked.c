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

int AttackingPieces(const BOARD *board, SQUARE square, COLOR attacking_color, SQUARE *attacking_sqs)
{
    int attackers = 0;
    SQUARE attacking_sq = 0;
    
    PIECE N_attacker, B_attacker, R_attacker, Q_attacker, K_attacker;
    
    if(attacking_color){
        for(int i = 0; w_pawn_capture[i]; i++){
            attacking_sq = square - w_pawn_capture[i];
            if(IN_BOARD(attacking_sq) && board->squares[attacking_sq] == W_PAWN){
                if(attacking_sqs) attacking_sqs[attackers++] = attacking_sq;
                else return 1;
            }
        }
        N_attacker = W_KNIGHT, B_attacker = W_BISHOP, R_attacker = W_ROOK,
        Q_attacker = W_QUEEN, K_attacker = W_KING;
    }else{
        for(int i = 0; b_pawn_capture[i]; i++){
            attacking_sq = square - b_pawn_capture[i];
            if(IN_BOARD(attacking_sq) && board->squares[attacking_sq] == B_PAWN){
                if(attacking_sqs) attacking_sqs[attackers++] = attacking_sq;
                else return 1;
            }
        }
        N_attacker = B_KNIGHT, B_attacker = B_BISHOP, R_attacker = B_ROOK,
        Q_attacker = B_QUEEN, K_attacker = B_KING;
    }

    for(int i = 0; knight_delta[i]; i++){
        attacking_sq = square + knight_delta[i];
        if(IN_BOARD(attacking_sq)){
            if(board->squares[attacking_sq] == N_attacker){
                if(attacking_sqs) attacking_sqs[attackers++] = attacking_sq;
                else return 1;
            }
        }
    }

    for(int i = 0; bishop_delta[i]; i++){
        for(attacking_sq = square + bishop_delta[i]; IN_BOARD(attacking_sq); attacking_sq += bishop_delta[i]){
            if(board->squares[attacking_sq] == EMPTY){
                continue;
            }else if(board->squares[attacking_sq] == B_attacker) {
                if(attacking_sqs) attacking_sqs[attackers++] = attacking_sq;
                else return 1;
                break;
            }else if(board->squares[attacking_sq] == Q_attacker){
                if(attacking_sqs) attacking_sqs[attackers++] = attacking_sq;
                else return 1;
                break;
            }else break; /*Piece other than bishop/queen, or different color, stop sliding*/
        }
    }

    for(int i = 0; rook_delta[i]; i++){
        for(attacking_sq = square + rook_delta[i]; IN_BOARD(attacking_sq); attacking_sq += rook_delta[i]){
            if(board->squares[attacking_sq] == EMPTY){
                continue;
            }else if(board->squares[attacking_sq] == R_attacker){
                if(attacking_sqs) attacking_sqs[attackers++] = attacking_sq;
                else return 1;
                break;
            }else if(board->squares[attacking_sq] == Q_attacker){
                if(attacking_sqs) attacking_sqs[attackers++] = attacking_sq;
                else return 1;
                break;
            }else break;
        }
    }
    
    for(int i = 0; king_delta[i]; i++){
        attacking_sq = square + king_delta[i];
        if(IN_BOARD(attacking_sq)){
            if(board->squares[attacking_sq] == K_attacker){
                if(attacking_sqs) attacking_sqs[attackers++] = attacking_sq;
                else return 1;
                break;
            }
        }
    }
    return attackers;
}
