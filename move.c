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

inline void RememberCaptured(MOVE *move, PIECE captured)
{
  /*captured piece: bits 20-23; 0 if EMPTY, o en passant capture:*/
  *move |= (captured << 20);
}

inline void RememberEP(MOVE *move, SQUARE en_passant)
{
  /*stores board->en_passant in order to undo.*/ 
  *move |= (en_passant << 24);
}

inline void RememberCastleRight(MOVE *move, unsigned char rights)
{
  /*Stores lost castle rights: 'bits promoted' set to 14 if long castle is lost, 1 if short, 15 if both.*/
  *move |= (rights << 16);
}

static void RemovePiece(BOARD *board, SQUARE sq)
{
  PIECE p = board->squares[sq];
  board->zobrist_key ^= zobkeys.zob_pieces[p][sq];
  if (p == W_PAWN || p == B_PAWN) {
    board->pawn_material[GET_COLOR(p)] -= Value(p);
    board->pawns[GET_COLOR(p)] = BitboardUnset(sq, board->pawns[GET_COLOR(p)]);
  } else if (p != EMPTY && p != W_KING && p != B_KING) {
    if (p == W_BISHOP || p == B_BISHOP) board->bishops[GET_COLOR(p)]--;
    board->piece_material[GET_COLOR(p)] -= Value(p);
  }
  board->squares[sq] = EMPTY;
}

static void DropPiece(BOARD *board, SQUARE sq, PIECE p)
{
  board->zobrist_key ^= zobkeys.zob_pieces[p][sq];
  if (p == W_PAWN || p == B_PAWN) {
    board->pawn_material[GET_COLOR(p)] += Value(p);
    board->pawns[GET_COLOR(p)] = BitboardSet(sq, board->pawns[GET_COLOR(p)]);
  } else if (p != EMPTY && p != W_KING && p != B_KING) {
    if (p == W_BISHOP || p == B_BISHOP) board->bishops[GET_COLOR(p)]++;
    board->piece_material[GET_COLOR(p)] += Value(p);
  }
  board->squares[sq] = p;
}

static inline void MoveWhitePawn(BOARD *board, SQUARE orig, SQUARE dest, const MOVE *curr_move)
{
  PIECE promoted = PROMMASK(*curr_move);
  /*promoted does NOT store the correct color, because algebraic notation does not ensure Q != q.*/

  if (ROW(dest) == EIGHT_ROW) {      /*Promotion*/
    DropPiece(board, dest, TURN_WHITE(promoted));
    board->en_passant = INVALID_SQ;  /*Invalid square: no pawn en passant.*/
  } else {
    DropPiece(board, dest, W_PAWN);
    if (ROW(dest) == FOURTH_ROW && ROW(orig) == SECOND_ROW) {
      board->en_passant = dest + ROW_DOWN;
    } else {
      /*Remove en passant captured pawn (we won't later).*/
      if (dest == board->en_passant) RemovePiece(board, dest + ROW_DOWN);
      board->en_passant = INVALID_SQ;
    }
  }
  board->rev_plies[board->ply] = 0;
}

static inline void MoveBlackPawn(BOARD *board, SQUARE orig, SQUARE dest, const MOVE *curr_move)
{
  PIECE promoted = PROMMASK(*curr_move);
  if (ROW(dest) == FIRST_ROW) {
    DropPiece(board, dest, TURN_BLACK(promoted));
    board->en_passant = INVALID_SQ;
  } else {
    DropPiece(board, dest, B_PAWN);
    if (ROW(dest) == FIFTH_ROW && ROW(orig) == SEVENTH_ROW) {
      board->en_passant = dest + ROW_UP;
    } else {
      if (dest == board->en_passant) RemovePiece(board, dest + ROW_UP);
      board->en_passant = INVALID_SQ;
    }
  }
  board->rev_plies[board->ply] = 0;
}

static inline void MoveWhiteKing(BOARD *board, SQUARE orig, SQUARE dest, MOVE *curr_move)
{
  DropPiece(board, dest, W_KING);
  board->wking_pos = dest;
  if (orig == e1) {
    if (dest == g1) {    /*Short castle*/
      RemovePiece(board, h1);
      DropPiece(board, f1, W_ROOK);
      board->w_castled = 1;
    }
    if (dest == c1) {    /*Long castle*/
      RemovePiece(board, a1);
      DropPiece(board, d1, W_ROOK);
      board->w_castled = 1;
    }
    if (board->wq_castle) RememberCastleRight(curr_move, Q_CASTLE_RIGHT);
    if (board->wk_castle) RememberCastleRight(curr_move, K_CASTLE_RIGHT);
  }
  board->wk_castle = 0, board->wq_castle = 0;
  board->en_passant = INVALID_SQ;
}

static inline void MoveBlackKing(BOARD *board, SQUARE orig, SQUARE dest, MOVE *curr_move)
{
  DropPiece(board, dest, B_KING);
  board->bking_pos = dest;
  if (orig == e8) {
    if (dest == g8) {
      RemovePiece(board, h8);
      DropPiece(board, f8, B_ROOK);
      board->b_castled = 1;
    }
    if (dest == c8) {
      RemovePiece(board, a8);
      DropPiece(board, d8, B_ROOK);
      board->b_castled = 1;
    }
    if (board->bq_castle) RememberCastleRight(curr_move, Q_CASTLE_RIGHT);
    if (board->bk_castle) RememberCastleRight(curr_move, K_CASTLE_RIGHT);
  }
  board->bk_castle = 0, board->bq_castle = 0;
  board->en_passant = INVALID_SQ;
}

static inline void MoveWhiteRook(BOARD *board, SQUARE orig, SQUARE dest, MOVE *curr_move)
{
  if (board->wq_castle && orig == a1) {
    RememberCastleRight(curr_move, Q_CASTLE_RIGHT);
    board->wq_castle = 0;
  }
  if (board->wk_castle && orig == h1) {
    RememberCastleRight(curr_move, K_CASTLE_RIGHT);
    board->wk_castle = 0;
  }
  DropPiece(board, dest, W_ROOK);
  board->en_passant = INVALID_SQ;
}

static inline void MoveBlackRook(BOARD *board, SQUARE orig, SQUARE dest, MOVE *curr_move)
{  
  if (board->bq_castle && orig == a8) {
    RememberCastleRight(curr_move, Q_CASTLE_RIGHT);
    board->bq_castle = 0;
  }
  if (board->bk_castle && orig == h8) {
    RememberCastleRight(curr_move, K_CASTLE_RIGHT);
    board->bk_castle = 0;
  }
  DropPiece(board, dest, B_ROOK);
  board->en_passant = INVALID_SQ;
}

/*
'MakeMove()' modifies curr_move to update information depending on the position (captured, castle, EP...).
'MakeMove()' does NOT verify legality!
*/
void MakeMove(BOARD *board, MOVE *curr_move)
{
  board->ply++;
  SQUARE orig = ORIGMASK(*curr_move);
  SQUARE dest = DESTMASK(*curr_move);

  RememberEP(curr_move, board->en_passant);
  
  /*don't update 'castle rights' or 'en passant' board->zobrist_key; useless?*/
  /*if (IN_BOARD(board->en_passant)) board->zobrist_key ^= zobkeys.zob_enpass[board->en_passant];*/

  if (board->squares[dest]) {
    RememberCaptured(curr_move, board->squares[dest]);
    RemovePiece(board, dest);
    board->rev_plies[board->ply] = 0;
  } else {
    board->rev_plies[board->ply] = board->rev_plies[board->ply-1]+1;  
  }
  
  switch (board->squares[orig]) {
    case W_PAWN:
      MoveWhitePawn(board, orig, dest, curr_move);
      break;
    case B_PAWN:
      MoveBlackPawn(board, orig, dest, curr_move);
      break;
    case W_KING:
      MoveWhiteKing(board, orig, dest, curr_move);
      break;
    case B_KING:
      MoveBlackKing(board, orig, dest, curr_move);
      break;
    case W_ROOK:
      MoveWhiteRook(board, orig, dest, curr_move);
      break;
    case B_ROOK:
      MoveBlackRook(board, orig, dest, curr_move);
      break;
    default:
      DropPiece(board, dest, board->squares[orig]);
      board->en_passant = INVALID_SQ;
      break;
  }
  
  RemovePiece(board, orig);
  /*if (IN_BOARD(board->en_passant)) board->zobrist_key ^= zobkeys.zob_enpass[board->en_passant];*/
  board->white_to_move = !(board->white_to_move);
  board->zobrist_key ^= zobkeys.zob_side;
  board->zobrist_history[board->ply] = board->zobrist_key;

}


/**************************************************************************************
  Takebacks.
***************************************************************************************/

static inline void TakebackWhiteKing(BOARD *board, SQUARE orig, SQUARE dest, PIECE promoted)
{
  DropPiece(board, orig, W_KING);
  board->wking_pos = orig;
  if (orig == e1) {
    if (dest == g1) {        /*short castle.*/
      RemovePiece(board, f1);
      DropPiece(board, h1, W_ROOK);
      board->wk_castle = 1;
      board->w_castled = 0;
    } else if (dest == c1) {      /*Long castle.*/
      RemovePiece(board, d1);
      DropPiece(board, a1, W_ROOK);
      board->wq_castle = 1;
      board->w_castled = 0;
    }
    if (promoted == Q_CASTLE_RIGHT || promoted == BOTH_CASTLES) {
      board->wq_castle = 1;
    }
    if (promoted == K_CASTLE_RIGHT || promoted == BOTH_CASTLES) {
      board->wk_castle = 1;  
    }
  }
}

static inline void TakebackBlackKing(BOARD *board, SQUARE orig, SQUARE dest, PIECE promoted)
{
  DropPiece(board, orig, B_KING);
  board->bking_pos = orig;
  if (orig == e8) {
    if (dest == g8) {
      RemovePiece(board, f8);
      DropPiece(board, h8, B_ROOK);
      board->bk_castle = 1;
      board->b_castled = 0;
    } else if (dest == c8) {
      RemovePiece(board, d8);
      DropPiece(board, a8, B_ROOK);
      board->bq_castle = 1;
      board->b_castled = 0;
    }
    if (promoted == Q_CASTLE_RIGHT || promoted == BOTH_CASTLES) {
      board->bq_castle = 1;
    }
    if (promoted == K_CASTLE_RIGHT || promoted == BOTH_CASTLES) {
      board->bk_castle = 1;
    }
  }
}

static inline void TakebackWhiteRook(BOARD *board, SQUARE orig, SQUARE dest, PIECE promoted)
{
  if (promoted) {
    if (promoted == Q_CASTLE_RIGHT) {
      board->wq_castle = 1;
      DropPiece(board, orig, W_ROOK);
    } else if (promoted == K_CASTLE_RIGHT) {
      board->wk_castle = 1;
      DropPiece(board, orig, W_ROOK);
    } else {
      DropPiece(board, orig, W_PAWN);
    }
  } else {
    DropPiece(board, orig, W_ROOK);
  }
}

static inline void TakebackBlackRook(BOARD *board, SQUARE orig, SQUARE dest, PIECE promoted)
{
  if (promoted) {
    if (promoted == Q_CASTLE_RIGHT) {
      board->bq_castle = 1;
      DropPiece(board, orig, B_ROOK);
    } else if (promoted == K_CASTLE_RIGHT) {
      board->bk_castle = 1;
      DropPiece(board, orig, B_ROOK);
    } else {        
      DropPiece(board, orig, B_PAWN);
    }
  } else {
    DropPiece(board, orig, B_ROOK);
  }
}

void Takeback(BOARD *board, const MOVE prev_move)
{
  SQUARE orig = ORIGMASK(prev_move);
  SQUARE dest = DESTMASK(prev_move);
  PIECE promoted = PROMMASK(prev_move);
  PIECE captured = CAPTMASK(prev_move);
  
  /*if (IN_BOARD(board->en_passant)) board->zobrist_key ^= zobkeys.zob_enpass[board->en_passant];*/
  board->en_passant = EPMASK(prev_move);
  /*if (IN_BOARD(board->en_passant)) board->zobrist_key ^= zobkeys.zob_enpass[board->en_passant];*/
  board->white_to_move = !(board->white_to_move);
  board->zobrist_key ^= zobkeys.zob_side;

  switch(board->squares[dest]) {
      case W_PAWN:
        if (dest == board->en_passant)  DropPiece(board, dest + ROW_DOWN, B_PAWN);
        DropPiece(board, orig, W_PAWN);
        break;
      case B_PAWN:
        if (dest == board->en_passant) DropPiece(board, dest + ROW_UP, W_PAWN);
        DropPiece(board, orig, B_PAWN);
        break;
      case W_KING:
        TakebackWhiteKing(board, orig, dest, promoted);
        break;
      case B_KING:
        TakebackBlackKing(board, orig, dest, promoted);
        break;
      case W_ROOK:
        TakebackWhiteRook(board, orig, dest, promoted);
        break;
      case B_ROOK:
        TakebackBlackRook(board, orig,  dest, promoted);
        break;

      default:
        if (promoted) {
          /*drop a PAWN, same color as the piece's on dest.*/
          DropPiece(board, orig, (GET_COLOR(board->squares[dest]) ? W_PAWN : B_PAWN));
        } else {
          DropPiece(board, orig, board->squares[dest]);
        }
        break;
  }
  RemovePiece(board, dest);
  if (captured) DropPiece(board, dest, captured);
  board->ply--;
}
