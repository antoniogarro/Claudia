
Claudia is an open source UCI chess engine written in C.

It is one (and the weakest) of the eight chess engines that were originally included with [DGT
Pi](http://www.digitalgametechnology.com/index.php/products/chess-computers/pi?mavikthumbnails_display_ratio=1.5)
and [Picochess](https://www.picochess.org) (until July 2017), alongside Stockfish,
Texel, Arasan, RodentII, Zurichess, Floyd, and Cinnamon. According to the user manual
for DGT Pi, Claudia is "an attractive opponent for intermediate club players" and "has
a remarkably human playing style".

From time to time, Claudia plays on the Free Internet Chess Server
([FICS](https://www.freechess.org)), where it has an authorised computer account under
the name of [Claudiae](https://www.ficsgames.org/cgi-bin/search.cgi?player=claudiae&action=Statistics)
(the operation is fully automated through my Python script
[ponderbot](https://github.com/antoniogarro/ponderbot)).

I wrote Claudia from scratch in order to learn a bit about chess programming. Although
there is much room to improve it, I consider Claudia a finished project which I do not
plan to keep developing in the near future.


Usage
-----

Claudia runs as a console application and can be used with any chess GUI that supports
the UCI protocol. I have compiled it with gcc (both 64 and 32 bits systems) and
Visual C++.


Algorithm
---------

The engine uses:

  * 0x88 internal board representation,
  * bitboards for pawn structure evaluation,
  * a single threaded negamax algorithm,
  * alpha-beta pruning,
  * iterative deepening,
  * aspiration windows,
  * principal variation search,
  * null move pruning,
  * lazy evaluation,
  * quiescent search,
  * check extensions,
  * static exchange evaluation,
  * killer moves heuristic
  * transposition tables,
  * pondering.

Its leaf evaluation function is somewhat rudimentary, taking only into account
material advantage, piece mobility and pawn structure, and using a tapered evaluation
with limited ending knowledge.

Claudia supports opening books in Polyglot (.bin) format, and will attempt to use
any opening book named "book.bin" and placed in its same directory. The default book
has been built from over forty thousand high-quality tournament games played by Grand
Masters and International Masters over five years.


Name
----

I obviously named Claudia after Claude E. Shannon (1916 - 2001).


Thanks
------

To the authors of the main references I have used: Bruce Moreland's late [web
page](https://web.archive.org/web/20070707012511/http://www.brucemo.com/compchess/programming/index.htm),
Mediocre chess [blog](https://mediocrechess.blogspot.com),
and the [Chess Programming Wiki](https://www.chessprogramming.org/Main_Page).

To the players and the staff of FICS alike, for creating an open place to play chess online
that provided invaluable data to debug the program.


Copyright
---------

This program has been written by Antonio Garro, and is released under a permissive
BSD license: the code can be used and modified, as long as proper attribution is given.
Please refer to the license in the source code for further details.

The Polyglot opening book support uses code written and released into the public domain
by Michel Van den Bergh.

The Mersenne Twister pseudorandom number generator was written by Makoto Matsumoto
and Takuji Nishimura (please see the copyright notice in random.h).

