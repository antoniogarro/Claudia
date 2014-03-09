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

#include "engine.h"
#include <string.h>

int ParseInput(char *command)
{	
	if(command){
		if(!strcmp(command, "perft")) return COM_PERFT;

		if(!strcmp(command, "go")) return COM_GO;

		if(!strcmp(command, "stop")) return COM_STOP;

		if(!strcmp(command, "position")) return COM_POSITION;

		if(!strcmp(command, "isready")) return COM_IS_READY;

		if(!strcmp(command, "uci")) return COM_UCI;

		if(!strcmp(command, "quit")) return COM_QUIT;

		move curr_move = AlgebToMove(command);
		if(curr_move) return COM_MOVE;

	}
	return COM_INVALID;
}

int ParseGoParams(char *pparams)
{
	if(pparams){
		if(!strcmp(pparams, "depth")) return GO_DEPTH;

		if(!strcmp(pparams, "movetime")) return GO_TIME;

		if(!strcmp(pparams, "infinite")) return GO_INFINITE;

		if(!strcmp(pparams, "wtime")) return W_TIME;

		if(!strcmp(pparams, "btime")) return B_TIME;

		if(!strcmp(pparams, "winc")) return W_INC;

		if(!strcmp(pparams, "binc")) return B_INC;

	}
	return COM_INVALID;
}
