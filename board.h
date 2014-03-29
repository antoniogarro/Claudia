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

#include "claudia.h"
#include "hashtable.h"
#include "pawns.h"

#define HISTLEN 500
#define BOARDSIZE 0x80

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

int WhitePawnMoves(const BOARD*, SQUARE, MOVE*, int, char);
int BlackPawnMoves(const BOARD*, SQUARE, MOVE*, int, char);
int SlidingMoves(const BOARD*, SQUARE, const char*, COLOR, MOVE*, int, char);
int NonSlidingMoves(const BOARD*, SQUARE, const char*, COLOR, MOVE*, int, char);
int GenerateWhiteCastle(const BOARD*, MOVE*, int);
int GenerateBlackCastle(const BOARD*, MOVE*, int);

int IsAttacked(const BOARD*, SQUARE, COLOR);
int AttackingPieces(const BOARD*, SQUARE, COLOR, SQUARE*);

void MakeMove(BOARD*, MOVE*);
void Takeback(BOARD*, const MOVE);
/*
void RemovePiece(BOARD*, SQUARE);
void DropPiece(BOARD*, SQUARE, PIECE);
*/
char IsLegal(BOARD*, MOVE*);
int Perft(BOARD*, int);

/*
int SEE(BOARD*, MOVE*);
int EvaluateMove(BOARD*, MOVE*, const MOVE);
*/
void SortMoves(BOARD*, MOVE*, int, MOVE[]);
int FilterWinning(BOARD*, MOVE*, int);

int LazyEval(const BOARD*);
int StaticEval(const BOARD*);
int Value(PIECE);

static char WhiteInCheck(const BOARD *board){
    return IsAttacked(board, board->wking_pos, BLACK);
}

static char BlackInCheck(const BOARD *board){
    return IsAttacked(board, board->bking_pos, WHITE);
}

static int InCheck(const BOARD *board, SQUARE *attacking_sqs){
    if(board->white_to_move) return AttackingPieces(board, board->wking_pos, BLACK, attacking_sqs);
    else return AttackingPieces(board, board->bking_pos, WHITE, attacking_sqs);
}

static char LeftInCheck(const BOARD *board){
    if(board->white_to_move) return IsAttacked(board, board->bking_pos, WHITE);
    else return IsAttacked(board, board->wking_pos, BLACK);
}

static inline MOVE Move(PIECE piece, SQUARE dest, SQUARE orig)
{
    return (piece << 16) | (dest << 8) | orig;
}

/* Some methods to convert coordinates to algebraic notation, and the other way round.*/
char CharToPiece(const char);
char PieceToChar(const char);
char CharToCoordinate(const char);
char RowCoordinateToChar(const char);
char ColumnCoordinateToChar(const char);
MOVE AlgebToMove(const char*);
void MoveToAlgeb(const MOVE, char*);
#endif
