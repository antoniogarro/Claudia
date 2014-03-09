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
	char squares[128];
	/*Coordinates "behind" the pawn that can be captured en passant, as in FEN:*/
    char en_passant;
    char wk_castle, wq_castle, bk_castle, bq_castle;
	char w_castled, b_castled;
	char wking_pos, bking_pos;
	char white_to_move;
	int ply;
	int rev_plies [500];
	unsigned long long zobrist_key;
	unsigned long long zobrist_history[500];
};
extern struct BOARD board;
struct ZOBKEYS {
	unsigned long long zob_pieces[16][128];
	unsigned long long zob_enpass[128];
	unsigned long long zob_castle[4];
	unsigned long long zob_side;
};
extern struct ZOBKEYS zobkeys;

void InitBoard();
void InitZobrist();

int ReadFEN(const char*);
void PrintBoard();

int MoveGen(move*);
int CaptureGen(move*);

int WhitePawnMoves(const char, move *, int);
int BlackPawnMoves(const char, move *, int);
int SlidingMoves(const char, const char*, const char, move*, int);
int NonSlidingMoves(const char, const char *, const char, move *, int);
int GenerateWhiteCastle(move*, int);
int GenerateBlackCastle(move*, int);
int WhitePawnCaptures(const char, move *, int);
int BlackPawnCaptures(const char, move *, int );
int SlidingCaptures(const char, const char *, const char, move*, int);
int NonSlidingCaptures(const char, const char *, const char, move *, int);


char IsAttacked(const char, const char);
int AttackingPieces(const char, const char, char *);

void MakeMove(move *);
void Takeback(const move);
void RemovePiece(const char);
void DropPiece(const char, const char);
char IsLegal(move *);

int Perft(const int);
int SEE(move *);
int EvaluateMove(move *, const move);
void SortMoves(move *, const int);
int FilterWinning(move *, const int);
int Material();
int StaticEval();
int Value(const char);

int PawnStaticVal(const char, const char);
int KnightStaticVal(const char, const char);
int BishopStaticVal(const char, const char);
int RookStaticVal(const char, const char);
int QueenStaticVal(const char, const char);
int KingStaticVal(const char, const char);

/*
static char SideToMove(){
	return board.white_to_move;
}
*/
static char WhiteInCheck(){
	return IsAttacked(board.wking_pos, BLACK_COLOR);
}

static char BlackInCheck(){
	return IsAttacked(board.bking_pos, WHITE_COLOR);
}

static int InCheck(char * attacking_sqs){
	if(board.white_to_move) return AttackingPieces(board.wking_pos, BLACK_COLOR, attacking_sqs);
	else return AttackingPieces(board.bking_pos, WHITE_COLOR, attacking_sqs);
}

static char LeftInCheck(){
	if(board.white_to_move)	return IsAttacked(board.bking_pos, WHITE_COLOR);
	else return IsAttacked(board.wking_pos, BLACK_COLOR);
}

/* Some methods to convert coordinates to algebraic notation, and the other way round.*/
char CharToPiece(const char);
char PieceToChar(const char);
char CharToCoordinate(const char);
char RowCoordinateToChar(const char);
char ColumnCoordinateToChar(const char);
move AlgebToMove(const char*);
void MoveToAlgeb(const move, char*);
#endif
