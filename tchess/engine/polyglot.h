/*
 * polyglot.h
 *
 * Credit to  Bluefever Software youtube channel whose video series
 * helped a lot with polyglot opening book, and to the creator of the opening book, codekiddy7.
 *
 * Another opening book is also in the res folder, which is much smaller.
 *
 *  Created on: 2021. febr. 6.
 *      Author: Gáspár Tamás
 */

#ifndef SRC_ENGINE_POLYGLOT_H_
#define SRC_ENGINE_POLYGLOT_H_

#include <iostream>

#include "board/board.h"

#ifdef _MSC_VER
#  define U64(u) (u##ui64)
#else
#  define U64(u) (u##ULL)
#endif

//Unsigned 64 bit integer type definition.
typedef unsigned long long uint64;

namespace tchess
{
	//Random array used by polyglot books AND the transposition table.
	extern const uint64 Random64[781];

	/*
	 * Represents one entry in a binary polyglot book.
	 */
	struct polyglot_book_entry {

		//Polyglot hash of the board.
		uint64 boardKey;

		//Polyglot hash of the move in this position.
		unsigned short move;

		//Weight of this move.
		unsigned short weight;

		//Learn factor, currently unused.
		unsigned int learnFactor;

	};

	/*
	 * Represents the polyglot opening book. At creation it will read the contents
	 * of the opening book into memory.
	 */
	class opening_book {

		//Amount of entries in the book.
		long entriesNumber;

		//Points to the entries in the memory.
		polyglot_book_entry* entries;

	public:
		opening_book();

		~opening_book();

		/*
		 * Extract a move from the opening book to the given board position. If there is no
		 * move found for the position, then the special invalid move (0,0,quietMove) is returned.
		 */
		move getBookMove(const chessboard& board, const game_information& info);
	};

	/*
	 * Creates the polygot hash key of a board. Can be used to
	 * look up positions in the opening book.
	 */
	uint64 polyKeyFromBoard(const chessboard& board, const game_information& info);

	/*
	 * Converts a piece code used in Tchess to a polyglot
	 * piece code.
	 */
	unsigned int polyPieceCode(int pieceCode);
}

#endif /* SRC_ENGINE_POLYGLOT_H_ */
