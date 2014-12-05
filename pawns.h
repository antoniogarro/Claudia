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
#ifndef PAWNSH
#define PAWNSH

#include "board.h"

typedef unsigned long long BITBOARD;

#define NOT_A 0xfefefefefefefefe
#define NOT_H 0x7f7f7f7f7f7f7f7f

static char BRANKS[] = {
  7,7,7,7,7,7,7,7,
  6,6,6,6,6,6,6,6,
  5,5,5,5,5,5,5,5,
  4,4,4,4,4,4,4,4,
  3,3,3,3,3,3,3,3,
  2,2,2,2,2,2,2,2,
  1,1,1,1,1,1,1,1,
  0,0,0,0,0,0,0,0
  };
  
static char WRANKS[] = {
  0,0,0,0,0,0,0,0,
  1,1,1,1,1,1,1,1,             
  2,2,2,2,2,2,2,2,
  3,3,3,3,3,3,3,3,
  4,4,4,4,4,4,4,4,
  5,5,5,5,5,5,5,5,
  6,6,6,6,6,6,6,6,
  7,7,7,7,7,7,7,7
  };

static inline BITBOARD BitboardSet(SQUARE sq, BITBOARD b)
{
  return b | (1ull << (ROW(sq)>>1) << COLUMN(sq));
}

static inline BITBOARD BitboardUnset(SQUARE sq, BITBOARD b)
{
  return b ^ (1ull << (ROW(sq)>>1) << COLUMN(sq));
}

static inline BITBOARD LSB(BITBOARD b)
{
  return b ^ (b-1);
}

static inline int BitCount(BITBOARD b)
{
  int count = 0;
  for (; b; count++) {
    b &= b-1;
  }
  return count;
}

static int DotProduct(BITBOARD b, char weights[])
{
   BITBOARD bit = 1;
   int p = 0;
   for (SQUARE sq = 0; b && sq < 64; sq++, bit += bit) {
    if (b & bit) p += weights[sq];
   }
   return p;
}

static inline int BitIndexLSB(BITBOARD b)
{
  static const BITBOARD B[] = {0xAAAAAAAA, 0xCCCCCCCC, 0xF0F0F0F0, 
                 0xFF00FF00, 0xFFFF0000, 0xFFFFFFFF00000000};
  unsigned int r = (b & B[0]) != 0;
  for (int i = 5; i > 0; i--) {
    r |= ((b & B[i]) != 0) << i;
  }
  return r;
}

static inline BITBOARD NorthFill(BITBOARD b)
{
  b |= (b <<  8);
  b |= (b << 16);
  b |= (b << 32);
  return b;
}
static inline BITBOARD SouthFill(BITBOARD b)
{
  b |= (b >>  8);
  b |= (b >> 16);
  b |= (b >> 32);
  return b;
}

static inline BITBOARD FileFill(BITBOARD b)
{
   return NorthFill(b) | SouthFill(b);
}

static inline BITBOARD SouthOne(BITBOARD b)
{
  return  b >> 8;
}

static inline BITBOARD NorthOne(BITBOARD b)
{
  return  b << 8;
}

static inline BITBOARD EastOne(BITBOARD b)
{
  return (b & NOT_H) << 1;
}

static inline BITBOARD WestOne(BITBOARD b)
{
  return (b & NOT_A) >> 1;
}

static inline BITBOARD WFrontSpans(BITBOARD wpawns)
{
  return NorthOne(NorthFill(wpawns));
}

static inline BITBOARD WRearSpans(BITBOARD wpawns)
{
  return SouthOne(SouthFill(wpawns));
}

static inline BITBOARD BFrontSpans(BITBOARD bpawns)
{
  return SouthOne(SouthFill(bpawns));
}

static inline BITBOARD BRearSpans(BITBOARD bpawns)
{
  return NorthOne(NorthFill(bpawns));
}

static inline BITBOARD WPassedPawns(BITBOARD wpawns, BITBOARD bpawns)
{
  BITBOARD front_spans = BFrontSpans(bpawns);
  front_spans |= EastOne(front_spans) | WestOne(front_spans);
  return wpawns & ~front_spans;
}
 
static inline BITBOARD BPassedPawns(BITBOARD bpawns, BITBOARD wpawns)
{
  BITBOARD front_spans = WFrontSpans(wpawns);
  front_spans |= EastOne(front_spans) | WestOne(front_spans);
  return bpawns & ~front_spans;
}

static inline BITBOARD DoubledPawns(BITBOARD pawns)
{
  return pawns & WRearSpans(pawns);
}
#endif
