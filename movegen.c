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

inline int WhitePawnMoves(const BOARD *board, SQUARE orig, MOVE *poss_moves, int nmoves, char noncaptures)
{
    SQUARE dest = orig + ROW_UP;
    if(noncaptures && IN_BOARD(dest) && board->squares[dest] == EMPTY){
        if(ROW(dest) == EIGHT_ROW){            /*Promotions.*/
            if(poss_moves) poss_moves[nmoves] = Move(W_QUEEN, dest, orig);
            nmoves++;
            if(poss_moves) poss_moves[nmoves] = Move(W_KNIGHT, dest, orig);
            nmoves++;
            if(poss_moves) poss_moves[nmoves] = Move(W_ROOK, dest, orig);
            nmoves++;
            if(poss_moves) poss_moves[nmoves] = Move(W_BISHOP, dest, orig);
            nmoves++;
        }else{
            if(poss_moves) poss_moves[nmoves] = Move(0, dest, orig);
            nmoves++;
            if(ROW(dest) == THIRD_ROW){
                dest = dest + ROW_UP;
                if(IN_BOARD(dest) && board->squares[dest] == EMPTY){
                    if(poss_moves) poss_moves[nmoves] = Move(0, dest, orig);
                    nmoves++;
                }
            }
        }
    }
    for(int i = 0; w_pawn_capture[i]; i++){
        dest = orig + w_pawn_capture[i];
        if(IN_BOARD(dest)){
            if(board->squares[dest] == EMPTY){
                if(dest == board->en_passant){        /*Captures en Passant.*/
                    if(poss_moves) poss_moves[nmoves] = Move(0, dest, orig);
                    nmoves++;
                }
            }else if(GET_COLOR(board->squares[dest]) == BLACK){
                if(ROW(dest) == EIGHT_ROW){
                    if(poss_moves) poss_moves[nmoves] = Move(W_QUEEN, dest, orig);
                    nmoves++;
                    if(poss_moves) poss_moves[nmoves] = Move(W_KNIGHT, dest, orig);
                    nmoves++;
                    if(poss_moves) poss_moves[nmoves] = Move(W_ROOK, dest, orig);
                    nmoves++;
                    if(poss_moves) poss_moves[nmoves] = Move(W_BISHOP, dest, orig);
                    nmoves++;
                }else{
                    if(poss_moves) poss_moves[nmoves] = Move(0, dest, orig);
                    nmoves++;
                }
            }
        }
    }
    return nmoves;
}

inline int BlackPawnMoves(const BOARD *board, SQUARE orig, MOVE *poss_moves, int nmoves, char noncaptures)
{
    SQUARE dest = orig + ROW_DOWN;
    if(noncaptures && IN_BOARD(dest) && board->squares[dest] == EMPTY){
        if(ROW(dest) == FIRST_ROW){
            if(poss_moves) poss_moves[nmoves] = Move(B_QUEEN, dest, orig);
            nmoves++;
            if(poss_moves) poss_moves[nmoves] = Move(B_KNIGHT, dest, orig);
            nmoves++;
            if(poss_moves) poss_moves[nmoves] = Move(B_ROOK, dest, orig);
            nmoves++;
            if(poss_moves) poss_moves[nmoves] = Move(B_BISHOP, dest, orig);
            nmoves++;
        }else{
            if(poss_moves) poss_moves[nmoves] = Move(0, dest, orig);
            nmoves++;
            if(ROW(dest) == SIXTH_ROW){
                dest = dest + ROW_DOWN;
                if(IN_BOARD(dest)){
                    if(board->squares[dest] == EMPTY){
                        if(poss_moves) poss_moves[nmoves] = Move(0, dest, orig);
                        nmoves++;
                    }
                }
            }
        }
    }
    for(int i = 0; b_pawn_capture[i]; i++){
        dest = orig + b_pawn_capture[i];
        if(IN_BOARD(dest)){
            if(board->squares[dest] == EMPTY){
                if(dest == board->en_passant){
                    if(poss_moves) poss_moves[nmoves] = Move(0, dest, orig);
                    nmoves++;
                }
            }else if(GET_COLOR(board->squares[dest])){
                if(ROW(dest) == FIRST_ROW){
                    if(poss_moves) poss_moves[nmoves] = Move(B_QUEEN, dest, orig);   
                    nmoves++;
                    if(poss_moves) poss_moves[nmoves] = Move(B_KNIGHT, dest, orig);
                    nmoves++;
                    if(poss_moves) poss_moves[nmoves] = Move(B_ROOK, dest, orig);
                    nmoves++;
                    if(poss_moves) poss_moves[nmoves] = Move(B_BISHOP, dest, orig);
                    nmoves++;
                }else{
                    if(poss_moves) poss_moves[nmoves] = Move(0, dest, orig);
                    nmoves++;
                }
            }
        }
    }
    return nmoves;
}

inline int SlidingMoves(const BOARD *board, SQUARE orig, MOVE *poss_moves, int nmoves, char noncaptures)
{
    PIECE p = board->squares[orig];
    for(const SQUARE *d = deltas[p]; *d; d++){
        for(SQUARE dest = orig + *d; IN_BOARD(dest); dest += *d){
            if(board->squares[dest] == EMPTY){
                if(noncaptures){
                    if(poss_moves) poss_moves[nmoves] = Move(0, dest, orig);
                    nmoves++;
                }
            }else if(GET_COLOR(board->squares[dest]) != GET_COLOR(p)) { 
                /*Different color Piece, capture, stop sliding.*/
                if(poss_moves) poss_moves[nmoves] = Move(0, dest, orig);
                nmoves++;
                break;
            }else break;        /*same color piece, stop sliding.*/
        }
    }
    return nmoves;
}

inline int NonSlidingMoves(const BOARD *board, SQUARE orig, MOVE *poss_moves, int nmoves, char noncaptures)
{
    PIECE p = board->squares[orig];
    for(const SQUARE *d = deltas[p]; *d; d++){
        SQUARE dest = orig + *d;
        if(IN_BOARD(dest)){
            if(board->squares[dest] == EMPTY){
                if(noncaptures){
                    if(poss_moves) poss_moves[nmoves] = Move(0, dest, orig);
                    nmoves++;
                }
            }else if(GET_COLOR(board->squares[dest]) != GET_COLOR(p)){
                if(poss_moves) poss_moves[nmoves] = Move(0, dest, orig);
                nmoves++;
            }
        }
    }
    return nmoves;
}

int GenerateWhiteCastle(const BOARD *board, MOVE *poss_moves, int nmoves)
{
    if(board->wk_castle && board->squares[h1] == W_ROOK && board->squares[g1] == EMPTY
            && board->squares[f1] == EMPTY && !WhiteInCheck(board)
            && !IsAttacked(board, f1, BLACK)){
        if(poss_moves) poss_moves[nmoves] = Move(0, g1, e1);
        nmoves++;
    }
    if(board->wq_castle && board->squares[a1] == W_ROOK &&board->squares[b1] == EMPTY
            && board->squares[c1] == EMPTY && board->squares[d1] == EMPTY
            && !WhiteInCheck(board) && !IsAttacked(board, d1, BLACK)){
        if(poss_moves) poss_moves[nmoves] = Move(0, c1, e1);
        nmoves++;
    }
    return nmoves;
}

int GenerateBlackCastle(const BOARD *board, MOVE *poss_moves, int nmoves)
{
    if(board->bk_castle && board->squares[h8] == B_ROOK &&board->squares[g8] == EMPTY
            && board->squares[f8] == EMPTY && !BlackInCheck(board)
            && !IsAttacked(board, f8, WHITE)){
        if(poss_moves) poss_moves[nmoves] = Move(0, g8, e8);
        nmoves++;
    }
    if(board->bq_castle && board->squares[a8] == B_ROOK &&board->squares[b8] == EMPTY
            && board->squares[c8] == EMPTY && board->squares[d8] == EMPTY
            && !BlackInCheck(board) && !IsAttacked(board, d8, WHITE)){
        if(poss_moves) poss_moves[nmoves] = Move(0, c8, e8);
        nmoves++;
    }
    return nmoves;
}

int CaptureGen(const BOARD *board, MOVE *poss_moves)
{
    return MoveGen(board, poss_moves, 0);
}
#define JUMP
#ifdef JUMP
int MoveGen(const BOARD *board, MOVE *poss_moves, char noncaptures)
{
    int nmoves = 0;
    for(SQUARE orig = a1; orig <= h8; orig++){
        PIECE p = board->squares[orig];
        if(p && GET_COLOR(p) == board->white_to_move){
            nmoves = piece_moves[p](board, orig, poss_moves, nmoves, noncaptures);
        }
        if(COLUMN(orig) == H_COLUMN) orig += 8;
    }
    if(noncaptures){
        if(board->white_to_move) nmoves = GenerateWhiteCastle(board, poss_moves, nmoves);
        else nmoves = GenerateBlackCastle(board, poss_moves, nmoves);
    }
    return nmoves;
}
#else
int MoveGen(const BOARD *board, MOVE *poss_moves, char noncaptures)
{
    int nmoves = 0;
    for(SQUARE orig = a1; orig <= h8; orig++){
        PIECE p = board->squares[orig];
        if(board->white_to_move){
            switch(p){
                case W_PAWN:
                    nmoves = WhitePawnMoves(board, orig, poss_moves, nmoves, noncaptures);
                    break;
                case W_KNIGHT:
                    nmoves = NonSlidingMoves(board, orig, poss_moves, nmoves, noncaptures);
                    break;
                case W_BISHOP:
                    nmoves = SlidingMoves(board, orig, poss_moves, nmoves, noncaptures);
                    break;
                case W_ROOK:
                    nmoves = SlidingMoves(board, orig, poss_moves, nmoves, noncaptures);
                    break;
                case W_QUEEN:
                    nmoves = SlidingMoves(board, orig, poss_moves, nmoves, noncaptures);
                    break;
                case W_KING:
                    nmoves = NonSlidingMoves(board, orig, poss_moves, nmoves, noncaptures);
                    break;
                default: break;
            }
            if(noncaptures) nmoves = GenerateWhiteCastle(board, poss_moves, nmoves);
        }else{
            switch(p){
                case B_PAWN:
                    nmoves = BlackPawnMoves(board, orig, poss_moves, nmoves, noncaptures);
                    break;
                case B_KNIGHT:
                    nmoves = NonSlidingMoves(board, orig, poss_moves, nmoves, noncaptures);
                    break;
                case B_BISHOP:
                    nmoves = SlidingMoves(board, orig, poss_moves, nmoves, noncaptures);
                    break;
                case B_ROOK:
                    nmoves = SlidingMoves(board, orig, poss_moves, nmoves, noncaptures);
                    break;
                case B_QUEEN:
                    nmoves = SlidingMoves(board, orig, poss_moves, nmoves, noncaptures);
                    break;
                case B_KING:
                    nmoves = NonSlidingMoves(board, orig, poss_moves, nmoves, noncaptures);
                    break;
                default: break;
            }
            if(noncaptures) nmoves = GenerateBlackCastle(board, poss_moves, nmoves);
        }
        if(COLUMN(orig) == H_COLUMN) orig += 8;
    }
    return nmoves;
}
#endif
