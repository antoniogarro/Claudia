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

#ifndef BOARDH
#define BOARDH

/*Type to store moves: ORIG:bits 0-7, DEST:bits 8-15, PROMOTED and CASTLE RIGHTS:bits 16-19,
CAPTURED:bits 20-23, PAWN_EP:bits 24-31 -> 0xEECPDDOO*/
typedef unsigned int MOVE;

typedef unsigned char SQUARE;
typedef unsigned char PIECE;
typedef unsigned char COLOR;

/*Zobrist keys*/
typedef unsigned long long KEY;

#define SQSMASK(move)  (move & 0xFFFF)
#define ORIGMASK(move) (move & 0xFF)
#define DESTMASK(move) ((move & 0xFFFF) >> 8)
#define PROMMASK(move) ((move & 0xFFFFF) >> 16)
#define CAPTMASK(move) ((move & 0xFFFFFF) >> 20)
#define EPMASK(move)   (move >> 24)

/* white pieces have least significant bit = 1; black = 0.*/
/* K_CASTLE_RIGHT | Q_CASTLE_RIGHT = BOTH_CASTLES */
enum PIECES { EMPTY,
              K_CASTLE_RIGHT,
              B_PAWN,
              W_PAWN,
              B_KNIGHT,
              W_KNIGHT,
              B_BISHOP,
              W_BISHOP,
              B_ROOK,
              W_ROOK,
              B_QUEEN,
              W_QUEEN,
              B_KING,
              W_KING,
              Q_CASTLE_RIGHT,
              BOTH_CASTLES
            };

#define WHITE 1
#define BLACK 0
#define TURN_BLACK(piece) (piece & 0xE)
#define TURN_WHITE(piece) (piece | 0x1)
#define BLACK_TO_COLOR(blackpiece, piece) ((blackpiece)|(piece & 0x1))
#define GET_COLOR(piece) (piece & 0x1)

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

#define ROW8(sq) ((sq) >> 4)

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

#define HISTLEN 500
#define BOARDSIZE 0x80

#include "hashtable.h"
#include "pawns.h"

typedef struct BOARD {
    PIECE squares[BOARDSIZE];
    BITBOARD pawns[2];
    COLOR white_to_move;
    /*Coordinates "behind" the pawn that can be captured en passant, as in FEN:*/
    SQUARE en_passant;
    unsigned char wk_castle, wq_castle, bk_castle, bq_castle;
    unsigned char w_castled, b_castled;
    SQUARE wking_pos, bking_pos;
    int ply;
    int rev_plies[HISTLEN];
    KEY zobrist_key;
    KEY zobrist_history[HISTLEN];
    unsigned int piece_material[2];
    unsigned int pawn_material[2];
} BOARD;

typedef struct ZOBKEYS {
    KEY zob_pieces[16][BOARDSIZE];
    KEY zob_enpass[BOARDSIZE];
    KEY zob_castle[4];
    KEY zob_side;
} ZOBKEYS;

extern ZOBKEYS zobkeys;

KEY PolyglotKey(const BOARD*);

void InitBoard(BOARD*);
void InitZobrist(BOARD*);
void InitMaterial(BOARD*);
void PrintBoard(const BOARD*);
int ReadFEN(const char*, BOARD*);

int MoveGen(const BOARD*, MOVE*, char);
int CaptureGen(const BOARD*, MOVE*);
int GenerateWhiteCastle(const BOARD*, MOVE*, int);
int GenerateBlackCastle(const BOARD*, MOVE*, int);

typedef int (PIECEMOVES)(const BOARD*, SQUARE, MOVE*, int, char);

PIECEMOVES WhitePawnMoves, BlackPawnMoves, NonSlidingMoves, SlidingMoves;

static PIECEMOVES *const piece_moves[] = {
    0,
    0,
    &BlackPawnMoves,
    &WhitePawnMoves,
    &NonSlidingMoves,
    &NonSlidingMoves,
    &SlidingMoves,
    &SlidingMoves,
    &SlidingMoves,
    &SlidingMoves,
    &SlidingMoves,
    &SlidingMoves,
    &NonSlidingMoves,
    &NonSlidingMoves,
};

#ifndef DELTAS
#define DELTAS
static const SQUARE w_pawn_capture[] = {0x0F, 0x11, 0};
static const SQUARE b_pawn_capture[] = {-0x0F, -0x11, 0};
static const SQUARE knight_delta[] = {0x21, 0x12, 0x1F, 0x0E, -0x12, -0x0E, -0x21, -0x1F, 0};
static const SQUARE bishop_delta[] = {0x11, 0x0F, -0x11, -0x0F, 0};
static const SQUARE rook_delta[] = {0x01, 0x10, -0x01, -0x10, 0};
static const SQUARE king_delta[] = {0x11, 0x0F, -0x11, -0x0F, 0x01, 0x10, -0x01, -0x10, 0};
#ifdef POINTDELTAS
static const SQUARE *const deltas[] = {
    0,
    0,
    b_pawn_capture,
    w_pawn_capture,
    knight_delta,
    knight_delta,
    bishop_delta,
    bishop_delta,
    rook_delta,
    rook_delta,
    king_delta,
    king_delta,
    king_delta,
    king_delta,
};
#else
static const SQUARE deltas[][9] = {
    {0,0,0,0,0,0,0,0,0,},
    {0,0,0,0,0,0,0,0,0,},
    {-0x0F, -0x11,0,0,0,0,0,0,0},
    {0x0F, 0x11,0,0,0,0,0,0,0},
    {0x21, 0x12, 0x1F, 0x0E, -0x12, -0x0E, -0x21, -0x1F, 0},
    {0x21, 0x12, 0x1F, 0x0E, -0x12, -0x0E, -0x21, -0x1F, 0},
    {0x11, 0x0F, -0x11, -0x0F,0,0,0,0,0},
    {0x11, 0x0F, -0x11, -0x0F,0,0,0,0,0},
    {0x01, 0x10, -0x01, -0x10,0,0,0,0,0},
    {0x01, 0x10, -0x01, -0x10,0,0,0,0,0},
    {0x11, 0x0F, -0x11, -0x0F, 0x01, 0x10, -0x01, -0x10, 0},
    {0x11, 0x0F, -0x11, -0x0F, 0x01, 0x10, -0x01, -0x10, 0},
    {0x11, 0x0F, -0x11, -0x0F, 0x01, 0x10, -0x01, -0x10, 0},
    {0x11, 0x0F, -0x11, -0x0F, 0x01, 0x10, -0x01, -0x10, 0}
};
#endif

static const SQUARE distance_to_center[] = {
    6, 5, 4, 3, 3, 4, 5, 6, 0, 0, 0, 0, 0, 0, 0, 0,
    5, 4, 3, 2, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0,
    4, 3, 2, 1, 1, 2, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 2, 1, 0, 0, 1, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 2, 1, 0, 0, 1, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0,
    4, 3, 2, 1, 1, 2, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0,
    5, 4, 3, 2, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0,
    6, 5, 4, 3, 3, 4, 5, 6, 0, 0, 0, 0, 0, 0, 0, 0
};
#endif

void MakeMove(BOARD*, MOVE*);
void Takeback(BOARD*, const MOVE);

char IsLegal(BOARD*, MOVE*);
int Perft(BOARD*, int);

void SortMoves(BOARD*, MOVE*, int, MOVE[]);
int FilterWinning(BOARD*, MOVE*, int);

int LazyEval(const BOARD*);
int StaticEval(const BOARD*);
int Value(PIECE);

int IsAttacked(const BOARD*, SQUARE, COLOR);
int AttackingPieces(const BOARD*, SQUARE, COLOR, SQUARE*);

inline int WhiteInCheck(const BOARD *board){
    return IsAttacked(board, board->wking_pos, BLACK);
}

inline int BlackInCheck(const BOARD *board){
    return IsAttacked(board, board->bking_pos, WHITE);
}

inline int InCheck(const BOARD *board, SQUARE *attacking_sqs){
    if(board->white_to_move) return AttackingPieces(board, board->wking_pos, BLACK, attacking_sqs);
    else return AttackingPieces(board, board->bking_pos, WHITE, attacking_sqs);
}

inline char LeftInCheck(const BOARD *board){
    if(board->white_to_move) return IsAttacked(board, board->bking_pos, WHITE);
    else return IsAttacked(board, board->wking_pos, BLACK);
}

inline MOVE Move(PIECE piece, SQUARE dest, SQUARE orig)
{
    return (piece << 16) | (dest << 8) | orig;
}

/* Some methods to convert coordinates to algebraic notation, and the other way round.*/
PIECE CharToPiece(const char);
char PieceToChar(PIECE);
char CharToCoordinate(const char);
char RowCoordinateToChar(const char);
char ColumnCoordinateToChar(const char);
MOVE AlgebToMove(const char*);
void MoveToAlgeb(const MOVE, char*);
#endif

