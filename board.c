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

#include <stdio.h>
#include "board.h"
#include "random.h"

void InitBoard(BOARD *board)
{
    unsigned long long init[4]={0x12345ULL,0x23456ULL,0x34567ULL,0x45678ULL};
    unsigned long long length = 4;
    init_by_array64(init, length);
    board->zobrist_key = 0;
    for(SQUARE sq = 0; sq < BOARDSIZE; sq++) {
        board->squares[sq] = EMPTY;
        /*Note j = 1; we want zobrist keys for EMPTY to be 0:*/
        for (int j = 1; j <= 0xF; j++){
            zobkeys.zob_pieces[j][sq] = genrand64_int64();
        }
        zobkeys.zob_enpass[sq] = genrand64_int64();
    }
    for(int i = 0; i < 4; i++) zobkeys.zob_castle[i] = genrand64_int64();
    zobkeys.zob_side = genrand64_int64();
    board->wk_castle = 0;
    board->bk_castle = 0;
    board->wq_castle = 0;
    board->bq_castle = 0;
    board->en_passant = INVALID_SQ;    /*there's no en passant pawn initialy.*/
    board->pawn_material[0] = 0, board->pawn_material[1] = 0;
    board->piece_material[0] = 0, board->piece_material[1] = 0;
}

void InitZobrist(BOARD *board)
{
    board->zobrist_key = 0;
    for (int i = 0; i < BOARDSIZE; i++){
        if(IN_BOARD(i)) board->zobrist_key ^= zobkeys.zob_pieces[board->squares[i]][i];
    }
    if(board->wk_castle) board->zobrist_key ^= zobkeys.zob_castle[0];
    if(board->wq_castle) board->zobrist_key ^= zobkeys.zob_castle[1];
    if(board->bk_castle) board->zobrist_key ^= zobkeys.zob_castle[2];
    if(board->bq_castle) board->zobrist_key ^= zobkeys.zob_castle[3];

    if(IN_BOARD(board->en_passant)) board->zobrist_key ^= zobkeys.zob_enpass[board->en_passant];
    if(board->white_to_move) board->zobrist_key ^= zobkeys.zob_side;
}

void InitMaterial(BOARD *board)
{
    board->pawn_material[0] = 0, board->pawn_material[1] = 0;
    board->piece_material[0] = 0, board->piece_material[1] = 0;
    board->pawns[0] = 0, board->pawns[1] = 0;
    for (int sq = 0; sq < BOARDSIZE; sq++){
        PIECE p = board->squares[sq];
        if(IN_BOARD(sq) && p != EMPTY && p != W_KING && p != B_KING){
            if(p == W_PAWN || p == B_PAWN){
                board->pawn_material[GET_COLOR(p)] += Value(p);
                board->pawns[GET_COLOR(p)] = BitboardSet(sq, board->pawns[GET_COLOR(p)]);
            }else{
                board->piece_material[GET_COLOR(p)] += Value(p);
            }
        }
    }
}

void PrintBoard(const BOARD *board)
{
    printf("\n");
    printf("\n+--+--+--+--+--+--+--+--+ %c %c%c%c%c   +--+--+--+--+--+--+--+--+    +--+--+--+--+--+--+--+--+\n",
            (board->white_to_move+1?'W':'B'),
            (board->wq_castle?'Q':' '), (board->wk_castle?'K':' '),
            (board->bq_castle?'q':' '), (board->bk_castle?'k':' '));
    
    for(int i = 0x70; i >= 0; i++){
        if(!(i&0x88)){
            printf("| %c", PieceToChar(board->squares[i]));
        }else{
            printf("|           ");
            for(int j = 0; j<8; j++) printf(" %c ", (board->pawns[0] & (1ull << (ROW(i-1) >> 1) << j)) ? 'p' : ' ');
            printf("     ");
            for(int j = 0; j<8; j++) printf(" %c ", (board->pawns[1] & (1ull << (ROW(i-1) >> 1) << j)) ? 'P' : ' ');
            printf("\n+--+--+--+--+--+--+--+--+          +--+--+--+--+--+--+--+--+    +--+--+--+--+--+--+--+--+\n");
            i -= 0x19;
        }
    }
    printf("Material: W: %i %i; B: %i %i; Zobrist: %Lu BB: %llx , %llx \n",
            board->piece_material[1], board->pawn_material[1],
            board->piece_material[0], board->pawn_material[0],
            board->zobrist_key, board->pawns[0], board->pawns[1]);
    printf("Passers: %llx ranks %i; %llx ranks %i  ",
            WPassedPawns(board->pawns[1], board->pawns[0]),
            DotProduct(WPassedPawns(board->pawns[1], board->pawns[0]), WRANKS),
            BPassedPawns(board->pawns[0], board->pawns[1]),
            DotProduct(BPassedPawns(board->pawns[0], board->pawns[1]), BRANKS));
    printf("Doubled: %i %i\n",
            BitCount(DoubledPawns(board->pawns[1])),
            BitCount(DoubledPawns(board->pawns[0])));
}
