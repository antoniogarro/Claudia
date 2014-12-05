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
#include "engine.h"
#include <stdio.h>
static inline int KingEval(const BOARD *board)
{
  return (board->w_castled * (1.0-PawnStage(board)) -
      board->b_castled * (1.0-PawnStage(board))) * CASTLE_BONUS
    +((board->wk_castle + board->wq_castle) -
      (board->bk_castle + board->bq_castle)) * CASTLE_RIGHT_BONUS
    +(distance_to_center[board->bking_pos] -
      distance_to_center[board->wking_pos]) * KING_CENTER_BONUS * PawnStage(board);
}

static inline int MobilityEval(const BOARD *board)
{
  int val = 0;
  for (SQUARE sq = a1; sq <= h8; sq++) {
    PIECE p = board->squares[sq];
    if (p && p < B_KING) {
      val += piece_moves[p](board, sq, 0, 0, 1) * mobility_bonus[p];
    }
    if (COLUMN(sq) == H_COLUMN) sq += 8;
  }
  return val;
}

static inline int PawnStructureEval(const BOARD *board)
{
  BITBOARD bp = board->pawns[BLACK], wp = board->pawns[WHITE];
  BITBOARD b = bp | wp;
  int val = GetPawnEval(&pawn_table, b);
  if (val != ERRORVALUE) return val;

  val = (BitCount(DoubledPawns(bp)) - BitCount(DoubledPawns(wp))) * DOUBLED_PAWN_BONUS;
  val += DotProduct(WPassedPawns(wp, bp), WRANKS) * PASSED_PAWN_BONUS;
  val -= DotProduct(BPassedPawns(bp, wp), BRANKS) * PASSED_PAWN_BONUS;
  
  val *= GameStage(board);
  UpdatePawnTable(&pawn_table, b, val);
  return val;
}

static inline int MaterialDraw(const BOARD *board)
{
  unsigned char side = board->white_to_move;
  if (board->pawn_material[side] || board->pawn_material[1-side]) return 0;
  if (board->piece_material[side] >= Value(W_ROOK) || board->piece_material[1-side] >= Value(W_ROOK)) return 0;
  return 1;
}

static inline int MaterialEval(const BOARD *board)
{
  return (board->piece_material[WHITE] + board->pawn_material[WHITE]
      - board->piece_material[BLACK] - board->pawn_material[BLACK])
      + ((board->bishops[WHITE] == 2) - (board->bishops[BLACK] == 2))
       * BISHOP_PAIR_BONUS * PawnStage(board);
}

int LazyEval(const BOARD *board)
{
  if (MaterialDraw(board)) return DRAW_VALUE;
  int lazy = MaterialEval(board) + PawnStructureEval(board);
  return board->white_to_move ? lazy : -lazy;
}

int StaticEval(const BOARD *board)
{
  if (MaterialDraw(board)) return DRAW_VALUE;
  int val = MaterialEval(board);
  val += MobilityEval(board);
  val += PawnStructureEval(board);
  val += KingEval(board);
  val *= (1.0 + SIMPLIFY_BONUS * GameStage(board));
  return board->white_to_move ? val : -val;
}

inline int Value(PIECE piece)
{
  return piece_values[piece];
}
