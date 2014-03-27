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

#ifndef CLAUDIAH
#define CLAUDIAH

#define NAME  "Claudia"
#define VERSION "0.2"

/*white pieces have last bit = 1; black or empty, = 0.*/
#define EMPTY 0x0            //0000            
#define W_PAWN 0x9           //1001            
#define B_PAWN 0x1           //0001            
#define W_KNIGHT 0xA         //1010        
#define B_KNIGHT 0x2         //0010    
#define W_BISHOP 0xB         //1011    
#define B_BISHOP 0x3         //0011    
#define W_ROOK 0xC           //1100    
#define B_ROOK 0x4           //0100
#define W_QUEEN 0xE          //1110
#define B_QUEEN 0x6          //0110
#define W_KING 0xF           //1111
#define B_KING 0x7           //0111
#define K_CASTLE_RIGHT 0x5   //0101
#define Q_CASTLE_RIGHT 0x8   //1000
#define BOTH_CASTLES 0xD     //1101

#define WHITE 0x8
#define BLACK 0x0
#define TURN_BLACK(piece) (piece & 0x7)
#define TURN_WHITE(piece) (piece | 0x8)
#define BLACK_TO_COLOR(blackpiece, piece) ((blackpiece)|(piece & 0x8))
#define GET_COLOR(piece) (piece & 0x8)  // WHITE or BLACK
#define GET_SIDE(piece) ((piece & 0x8) >> 3)    // 1 or 0

#define FIRST_ROW 0x00
#define SECOND_ROW 0x10
#define THIRD_ROW 0x20
#define FOURTH_ROW 0x30
#define FIFTH_ROW 0x40
#define SIXTH_ROW 0x50
#define SEVENTH_ROW 0x60
#define EIGHT_ROW 0x70

#define A_COLUMN 0x00
#define B_COLUMN 0x01
#define C_COLUMN 0x02
#define D_COLUMN 0x03
#define E_COLUMN 0x04
#define F_COLUMN 0x05
#define G_COLUMN 0x06
#define H_COLUMN 0x07

#define COLUMN(square) ((square) & 0x0F)
#define ROW(square) ((square) & 0xF0)
#define SQUARE(row, column) (row + column)
#define IN_BOARD(square) !((square) & 0x88)
#define INVALID_SQ 0xFF
#define NULL_MOVE 0xFFFF

#define a1 0x00
#define b1 0x01
#define c1 0x02
#define d1 0x03
#define e1 0x04
#define f1 0x05
#define g1 0x06
#define h1 0x07
#define a8 0x70
#define b8 0x71
#define c8 0x72
#define d8 0x73
#define e8 0x74
#define f8 0x75
#define g8 0x76
#define h8 0x77

#define STARTPOS "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\n"

#define ROW_UP 0x10
#define ROW_DOWN -0x10

#ifndef DELTAS
#define DELTAS
static char w_pawn_capture[] = {0x0F, 0x11, 0};
static char b_pawn_capture[] = {-0x0F, -0x11, 0};
static char knight_delta[] = {0x21, 0x12, 0x1F, 0x0E, -0x12, -0x0E, -0x21, -0x1F, 0};
static char bishop_delta[] = {0x11, 0x0F, -0x11, -0x0F, 0};
static char rook_delta[] = {0x01, 0x10, -0x01, -0x10, 0};
static char king_delta[] = {0x11, 0x0F, -0x11, -0x0F, 0x01, 0x10, -0x01, -0x10, 0};
#endif

#ifndef PIECE_VALUE
#define PIECE_VALUE
#define PAWN_VALUE 100
#define KNIGHT_VALUE 300
#define BISHOP_VALUE 320
#define ROOK_VALUE 500
#define QUEEN_VALUE 900
#define KING_VALUE 5000
#define INFINITE 10000000
#define DRAW_VALUE 0
#define MATE_VALUE -100000
#define HASHMOVE_VALUE 100000

#define ERRORVALUE -1000000001

#define N_MOBILITY_BONUS 5
#define B_MOBILITY_BONUS 4
#define R_MOBILITY_BONUS 1
#define Q_MOBILITY_BONUS 1
#define CASTLE_BONUS 60
#define CASTLE_RIGHT_BONUS 20
#endif

#ifndef SEARCH
#define SEARCH
#define ASP_WINDOW 50
#define LAZYBETA 400
#define LAZYALPHA 1000
#define MAXMOVES 250
#endif

/*Type to store moves: ORIG:bits 0-7, DEST:bits 8-15, PROMOTED:bits 16-19,
CAPTURED:bits 20-23, CASTLE RIGHTS, PAWN_EP:bits 24-31*/
typedef unsigned int MOVE;

/*Zobrist keys*/
typedef unsigned long long KEY;

typedef unsigned char SQUARE;
typedef unsigned char PIECE;
typedef char COLOR;

#define SQSMASK(move)  (move & 0xFFFF)
#define ORIGMASK(move) (move & 0xFF)
#define DESTMASK(move) ((move & 0xFFFF) >> 8)
#define PROMMASK(move) ((move & 0xFFFFF) >> 16)
#define CAPTMASK(move) ((move & 0xFFFFFF) >> 20)
#define EPMASK(move)   (move >> 24)

#endif
