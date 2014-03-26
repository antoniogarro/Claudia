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

struct BOARD {
    PIECE squares[128];
    /*Coordinates "behind" the pawn that can be captured en passant, as in FEN:*/
    SQUARE en_passant;
    unsigned char wk_castle, wq_castle, bk_castle, bq_castle;
    unsigned char w_castled, b_castled;
    SQUARE wking_pos, bking_pos;
    unsigned char white_to_move;
    int ply;
    int rev_plies [500];
    KEY zobrist_key;
    KEY zobrist_history[500];
    unsigned int piece_material[2];
    unsigned int pawn_material[2];
};
extern struct BOARD board;

struct ZOBKEYS {
    KEY zob_pieces[16][128];
    KEY zob_enpass[128];
    KEY zob_castle[4];
    KEY zob_side;
};
extern struct ZOBKEYS zobkeys;

void InitBoard();
void InitZobrist();
void InitMaterial();

int ReadFEN(const char*);
void PrintBoard();

int MoveGen(MOVE*, char);
int CaptureGen(MOVE*);

int WhitePawnMoves(SQUARE, MOVE*, int, char);
int BlackPawnMoves(SQUARE, MOVE*, int, char);
int SlidingMoves(SQUARE, const char*, char, MOVE*, int, char);
int NonSlidingMoves(SQUARE, const char*, char, MOVE*, int, char);
int GenerateWhiteCastle(MOVE*, int);
int GenerateBlackCastle(MOVE*, int);

char IsAttacked(SQUARE, COLOR);
int AttackingPieces(SQUARE, COLOR, SQUARE*);

void MakeMove(MOVE*);
void Takeback(const MOVE);
void RemovePiece(SQUARE);
void DropPiece(SQUARE, PIECE);
char IsLegal(MOVE*);

int Perft(int);
int SEE(MOVE*);
int EvaluateMove(MOVE*, const MOVE);
void SortMoves(MOVE*, int);
int FilterWinning(MOVE*, int);
int Material();
int LazyEval();
int StaticEval();
int Value(PIECE);

int PawnStaticVal(SQUARE, COLOR);
int KnightStaticVal(SQUARE, COLOR);
int BishopStaticVal(SQUARE, COLOR);
int RookStaticVal(SQUARE, COLOR);
int QueenStaticVal(SQUARE, COLOR);
int KingStaticVal(SQUARE, COLOR);

static char WhiteInCheck(){
    return IsAttacked(board.wking_pos, BLACK);
}

static char BlackInCheck(){
    return IsAttacked(board.bking_pos, WHITE);
}

static int InCheck(SQUARE *attacking_sqs){
    if(board.white_to_move) return AttackingPieces(board.wking_pos, BLACK, attacking_sqs);
    else return AttackingPieces(board.bking_pos, WHITE, attacking_sqs);
}

static char LeftInCheck(){
    if(board.white_to_move) return IsAttacked(board.bking_pos, WHITE);
    else return IsAttacked(board.wking_pos, BLACK);
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
