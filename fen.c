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
#include <stdlib.h>

int ReadFEN(const char *sFEN, BOARD *board)
{   /*TODO: check FEN validity.*/
  int fen_pos = 0;
  SQUARE square = 0x70;
  board->wk_castle = 0;
  board->bk_castle = 0;
  board->wq_castle = 0;
  board->bq_castle = 0;
  board->w_castled = 0;
  board->b_castled = 0;

  for (char on_board = 1; on_board; fen_pos++) {
    char empty = 0;
    switch (sFEN[fen_pos]) {
      case ' ': on_board = 0;
        break;      
      case '/': square -= 0x18;
        break;
      case '1': empty = 1;
        break;
      case '2':  empty = 2;
        break;
      case '3': empty = 3;
        break;
      case '4': empty = 4;
        break;
      case '5': empty = 5;
        break;
      case '6': empty = 6;
        break;
      case '7': empty = 7;
        break;
      case '8': empty = 8;
        break;
      default:
        board->squares[square] = CharToPiece(sFEN[fen_pos]);
        if (board->squares[square] == W_KING) board->wking_pos = square;
        else if (board->squares[square] == B_KING) board->bking_pos = square;
        square++;
        break;
    }
    for (int i = 0; i < empty; i++) board->squares[square+i] = EMPTY;
    square += empty;
  }

  board->white_to_move = (sFEN[fen_pos] == 'w');
  fen_pos+=2;
  
  for (char on_board = 1; on_board; fen_pos++) {    /*Castle rights loop.*/
    switch (sFEN[fen_pos]) {      
      case 'K':  board->wk_castle = 1;
        break;
      case 'k':  board->bk_castle = 1;
        break;
      case 'Q':  board->wq_castle = 1;
        break;
      case 'q':  board->bq_castle = 1;
        break;
      case '-':
        break;
      default: on_board = 0;
        break;
    }
  }
  /*Store En Passant coordinates. TODO: check.*/
  board->en_passant = 0x00;
  for (char on_board = 1; on_board; fen_pos++) {
    switch (sFEN[fen_pos]) {      
      case ' ': on_board = 0;
        break;
      default:
        board->en_passant += CharToCoordinate(sFEN[fen_pos]);
        break;
    }
  }
  /*halfmoves and moves.*/
  int rev = atoi(&sFEN[fen_pos]);
  while (sFEN[++fen_pos] != ' '); 
  board->ply = 2*atoi(&sFEN[fen_pos]) - board->white_to_move - 1;
  board->rev_plies[board->ply] = rev;
  InitZobrist(board);
  board->zobrist_history[board->ply] = board->zobrist_key;
  InitMaterial(board);
  return fen_pos;
}
