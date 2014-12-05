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

#ifndef HASHTABLEH
#define HASHTABLEH

#include "board.h"
#include "pawns.h"

#define HASH_EXACT 0
#define HASH_BETA 1
#define HASH_ALPHA 2

#define PUT_HASH_MOVE(move) ((KEY)(move & 0xFFFFF));
#define PUT_HASH_EVAL(eval) (((KEY)eval & 0xFFFFFFFF) << 20);
#define PUT_HASH_DEPTH(depth) (((KEY)depth & 0x3FF)<< 52);
#define PUT_HASH_FLAG(flag) ((KEY)flag << 62);

#define MOVEMASK(data) (MOVE)(data & 0xFFFFF)
#define EVALMASK(data) (int)(data >> 20)
#define DEPTHMASK(data) (int)((data >> 52) & 0x3FF)
#define FLAGMASK(data) (char)(data >> 62)

#define TABLESIZE 32

typedef struct{
  KEY zobrist_key;
  /*bits 0-19: hash_move; bits 20-51: eval; bits 52-61: depth; 62-63: flags.*/
  KEY data;
} HashData;

typedef struct HASHTABLE{
  HashData *entries;
  int size;
  int full;
} HASHTABLE;
extern struct HASHTABLE hash_table;

int AllocTable(HASHTABLE*, int);
void DeleteTable(HASHTABLE*);
void ClearHashTable(HASHTABLE*);
void UpdateTable(HASHTABLE*, KEY, int, MOVE, int, int);
MOVE GetHashMove(HASHTABLE*, KEY);
int GetHashEval(HASHTABLE*, KEY, int, int, int);

typedef struct{
  KEY pawn_bitboard;
  int eval;
} PawnData;

typedef struct PAWNTABLE{
  PawnData *entries;
  int size;
  int full;
} PAWNTABLE;

extern struct PAWNTABLE pawn_table;

int AllocPawnTable(PAWNTABLE*, int);
void DeletePawnTable(PAWNTABLE*);
void ClearPawnTable(PAWNTABLE*);
void UpdatePawnTable(PAWNTABLE*, BITBOARD, int);
int GetPawnEval(PAWNTABLE*, BITBOARD);
#endif
