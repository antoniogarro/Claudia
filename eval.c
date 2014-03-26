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

int LazyEval()
{
    unsigned char side = board.white_to_move;
    return board.piece_material[side] - board.piece_material[1-side]
         + board.pawn_material[side] - board.pawn_material[1-side];
}

int StaticEval()
{
    int val = 0;
    for(SQUARE sq = 0; sq<0x78; sq++){
        switch(board.squares[sq]){
            case EMPTY:
                break;
            case W_PAWN:
                val += PawnStaticVal(sq, WHITE_COLOR);
                break;
            case W_KNIGHT:
                val += KnightStaticVal(sq, WHITE_COLOR);
                break;
            case W_BISHOP:
                val += BishopStaticVal(sq, WHITE_COLOR);
                break;
            case W_ROOK:
                val += RookStaticVal(sq, WHITE_COLOR);
                break;
            case W_QUEEN:
                val += QueenStaticVal(sq, WHITE_COLOR);
                break;
            case W_KING:
                val += KingStaticVal(sq, WHITE_COLOR);
                break;

            case B_PAWN:
                val -= PawnStaticVal(sq, BLACK_COLOR);
                break;
            case B_KNIGHT:
                val -= KnightStaticVal(sq, BLACK_COLOR);
                break;
            case B_BISHOP:
                val -= BishopStaticVal(sq, BLACK_COLOR);
                break;
            case B_ROOK:
                val -= RookStaticVal(sq, BLACK_COLOR);
                break;
            case B_QUEEN:
                val -= QueenStaticVal(sq, BLACK_COLOR);
                break;
            case B_KING:
                val -= KingStaticVal(sq, BLACK_COLOR);
                break;
            default:
                break;
        }
        if(COLUMN(sq) == H_COLUMN && ROW(sq) != EIGHT_ROW) sq += 8;
    }
    if(board.white_to_move) return val;
    else return -val;
}

int PawnStaticVal(SQUARE sq, unsigned char color)
{
    int val = PAWN_VALUE;
    if(color) val += WhitePawnMoves(sq, 0, 0, 1);
    else val += BlackPawnMoves(sq, 0, 0, 1);
    return val;
}

int KnightStaticVal(SQUARE sq, unsigned char color)
{
    int val = KNIGHT_VALUE;
    val += N_MOBILITY_BONUS*NonSlidingMoves(sq, knight_delta, color, 0, 0, 1);
    return val;
}

int BishopStaticVal(SQUARE sq, unsigned char color)
{
    int val = BISHOP_VALUE;
    val += B_MOBILITY_BONUS*SlidingMoves(sq, bishop_delta, color, 0, 0, 1);
    return val;
}

int RookStaticVal(SQUARE sq, unsigned char color)
{
    int val = ROOK_VALUE;
    val += R_MOBILITY_BONUS*SlidingMoves(sq, rook_delta, color, 0, 0, 1);
    return val;
}

int QueenStaticVal(SQUARE sq, unsigned char color)
{
    int val = QUEEN_VALUE;
    val += Q_MOBILITY_BONUS*SlidingMoves(sq, king_delta, color, 0, 0, 1);
    return val;
}

int KingStaticVal(SQUARE sq, unsigned char color)
{
    int val = KING_VALUE;
    if(color){
        val += CASTLE_BONUS*board.w_castled;
        val += CASTLE_RIGHT_BONUS*(board.wk_castle + board.wq_castle);
    }else{
        val += CASTLE_BONUS*board.b_castled;
        val += CASTLE_RIGHT_BONUS*(board.bk_castle + board.bq_castle);
    }
    return val;
}
