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

int PawnStaticVal(const BOARD *board, SQUARE sq, COLOR color)
{
    float pawn_stage = (STARTPAWNS - board->pawn_material[0] - board->pawn_material[1])/STARTPAWNS;
    int val = PAWN_VALUE;
    if(color){
        val += ROW(sq)*pawn_stage*PAWN_PUSH_BONUS;
        val += WhitePawnMoves(board, sq, 0, 0, 1);
    }else{
        val += (EIGHT_ROW - ROW(sq))*pawn_stage*PAWN_PUSH_BONUS;
        val += BlackPawnMoves(board, sq, 0, 0, 1);
    }
    return val;
}

int KnightStaticVal(const BOARD *board, SQUARE sq, COLOR color)
{
    int val = KNIGHT_VALUE;
    val += N_MOBILITY_BONUS*NonSlidingMoves(board, sq, knight_delta, color, 0, 0, 1);
    return val;
}

int BishopStaticVal(const BOARD *board, SQUARE sq, COLOR color)
{
    int val = BISHOP_VALUE;
    val += B_MOBILITY_BONUS*SlidingMoves(board, sq, bishop_delta, color, 0, 0, 1);
    return val;
}

int RookStaticVal(const BOARD *board, SQUARE sq, COLOR color)
{
    int val = ROOK_VALUE;
    val += R_MOBILITY_BONUS*SlidingMoves(board, sq, rook_delta, color, 0, 0, 1);
    return val;
}

int QueenStaticVal(const BOARD *board, SQUARE sq, COLOR color)
{
    int val = QUEEN_VALUE;
    val += Q_MOBILITY_BONUS*SlidingMoves(board, sq, king_delta, color, 0, 0, 1);
    return val;
}

int KingStaticVal(const BOARD *board, SQUARE sq, COLOR color)
{
    int val = KING_VALUE;
    if(color){
        val += CASTLE_BONUS*board->w_castled;
        val += CASTLE_RIGHT_BONUS*(board->wk_castle + board->wq_castle);
    }else{
        val += CASTLE_BONUS*board->b_castled;
        val += CASTLE_RIGHT_BONUS*(board->bk_castle + board->bq_castle);
    }
    return val;
}

int PawnStructureEval(const BOARD *board)
{
    int dp_bonus = 0, isl_bonus = 0, chain[2] = {0,0}, npawns[2] = {0,0};
    for (int c = 0; c < 8; c++){ 
        
        npawns[1] = board->pawn_column[1][c];
        if(npawns[1] == 0) chain[1] = 0;
        else if (npawns[1] == 1){
            isl_bonus += chain[1];
            chain[1]++;
        }else{
            chain[1]++;
            dp_bonus -= npawns[1] * npawns[1];
        }
        
        npawns[0] = board->pawn_column[0][c];
        if(npawns[0] == 0) chain[0] = 0;
        else if (npawns[0] == 1){
            isl_bonus -= chain[0];
            chain[0]++;
        }else{
            chain[0]++;
            dp_bonus += npawns[0] * npawns[0];
        }
    }
    dp_bonus *= DOUBLED_PAWN_BONUS;
    isl_bonus *= ISOLATED_PAWN_BONUS;
    
    float pawn_stage = (STARTPAWNS - board->pawn_material[0] - board->pawn_material[1])/STARTPAWNS;
    return (dp_bonus + isl_bonus)*pawn_stage;
}

int MaterialDraw(const BOARD *board)
{
    unsigned char side = board->white_to_move;
    if(board->pawn_material[side] || board->pawn_material[1-side]) return 0;
    if(board->piece_material[side] >= ROOK_VALUE || board->piece_material[1-side] >= ROOK_VALUE) return 0;
    return 1;
}

int LazyEval(const BOARD *board)
{
    if(MaterialDraw(board)) return DRAW_VALUE;
    unsigned char side = board->white_to_move;
    int lazy = board->piece_material[1] - board->piece_material[0]
                + board->pawn_material[1] - board->pawn_material[0]
                + PawnStructureEval(board);
    if(side) return lazy;
    else return -lazy;
}

int StaticEval(const BOARD *board)
{
    if(MaterialDraw(board)) return DRAW_VALUE;
    int val = 0;
    for(SQUARE sq = 0; sq<0x78; sq++){
        switch(board->squares[sq]){
            case EMPTY:
                break;
            case W_PAWN:
                val += PawnStaticVal(board, sq, WHITE);
                break;
            case W_KNIGHT:
                val += KnightStaticVal(board, sq, WHITE);
                break;
            case W_BISHOP:
                val += BishopStaticVal(board, sq, WHITE);
                break;
            case W_ROOK:
                val += RookStaticVal(board, sq, WHITE);
                break;
            case W_QUEEN:
                val += QueenStaticVal(board, sq, WHITE);
                break;
            case W_KING:
                val += KingStaticVal(board, sq, WHITE);
                break;

            case B_PAWN:
                val -= PawnStaticVal(board, sq, BLACK);
                break;
            case B_KNIGHT:
                val -= KnightStaticVal(board, sq, BLACK);
                break;
            case B_BISHOP:
                val -= BishopStaticVal(board, sq, BLACK);
                break;
            case B_ROOK:
                val -= RookStaticVal(board, sq, BLACK);
                break;
            case B_QUEEN:
                val -= QueenStaticVal(board, sq, BLACK);
                break;
            case B_KING:
                val -= KingStaticVal(board, sq, BLACK);
                break;
            default:
                break;
        }
        if(COLUMN(sq) == H_COLUMN && ROW(sq) != EIGHT_ROW) sq += 8;
    }
    val += PawnStructureEval(board);
    
    if(board->white_to_move) return val;
    else return -val;
}

int Value(PIECE piece)
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
