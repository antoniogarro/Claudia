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
#include "claudia.h"
#include "board.h"
#include "random.h"

void InitBoard()
{
    unsigned long long init[4]={0x12345ULL,0x23456ULL,0x34567ULL,0x45678ULL};
    unsigned long long length = 4;
    init_by_array64(init, length);
    board.zobrist_key = 0;
    for(int i = 0; i<128; i++) {
        board.squares [i] = EMPTY;
        /*Note j = 1; we want zobrist keys for EMPTY to be 0:*/
        /*TODO: pre-generate*/
        for (int j = 1; j <= 0xF; j++){
            zobkeys.zob_pieces[j][i] = genrand64_int64();
        }
        zobkeys.zob_enpass[i] = genrand64_int64();
    }
    for(int i = 0; i < 4; i++) zobkeys.zob_castle[i] = genrand64_int64();
    zobkeys.zob_side = genrand64_int64();
    board.wk_castle = 0;
    board.bk_castle = 0;
    board.wq_castle = 0;
    board.bq_castle = 0;
    board.en_passant = 0x0F;        /*Invalid square, as there's no en passant pawn initialy.*/
    board.pawn_material[0] = 0, board.pawn_material[1] = 0;
    board.piece_material[0] = 0, board.piece_material[1] = 0;
}

void InitZobrist()
{
    board.zobrist_key = 0;
    for (int i = 0; i<128; i++){
        if(IN_BOARD(i)) board.zobrist_key ^= zobkeys.zob_pieces[board.squares[i]][i];
    }
    if(board.wk_castle) board.zobrist_key ^= zobkeys.zob_castle[0];
    if(board.wq_castle) board.zobrist_key ^= zobkeys.zob_castle[1];
    if(board.bk_castle) board.zobrist_key ^= zobkeys.zob_castle[2];
    if(board.bq_castle) board.zobrist_key ^= zobkeys.zob_castle[3];

    if(IN_BOARD(board.en_passant)) board.zobrist_key ^= zobkeys.zob_enpass[board.en_passant];
    if(board.white_to_move) board.zobrist_key ^= zobkeys.zob_side;
}

void InitMaterial()
{
    board.pawn_material[0] = 0, board.pawn_material[1] = 0;
    board.piece_material[0] = 0, board.piece_material[1] = 0;
    for (int i = 0; i<128; i++){
        PIECE p = board.squares[i];
        if(IN_BOARD(i) && p != EMPTY && p != W_KING && p != B_KING){
            if(TURN_WHITE(p) == W_PAWN){
                board.pawn_material[GET_SIDE(p)] += Value(p);
            }else{
                board.piece_material[GET_SIDE(p)] += Value(p);
            }
        }
    }
}

void PrintBoard()
{
    printf("\n+--+--+--+--+--+--+--+--+     %c %i%i\n",
            board.white_to_move+1, board.bq_castle, board.bk_castle);
    for(int i = 0x70; i>=0; i++){
        if(!(i&0x88)){
            printf("| %c", PieceToChar(board.squares[i]));
        }else{
            printf("|\n+--+--+--+--+--+--+--+--+\n");
            i -= 0x19;
        }
    }
    printf("                                %i%i   Zobrist: %Lu\n",
            board.wq_castle, board.wk_castle, board.zobrist_key);
    printf("Material: W: %i %i; B: %i %i\n", board.piece_material[1], board.pawn_material[1],
                                             board.piece_material[0], board.pawn_material[0]);
}
