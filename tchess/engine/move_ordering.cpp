/*
 * move_ordering.cpp
 *
 *  Created on: 2021. febr. 10.
 *      Author: Gáspár Tamás
 */
#include <stack>
#include <iostream>

#include "move_ordering.h"

namespace tchess
{

	struct move_data {
		move m;
		unsigned int side;
		int capturedPiece;
		move_data(const move& m, unsigned int side, int capt) : m(m), side(side), capturedPiece(capt) {}
	};

	void printPrincipalVariation(chessboard& board, const game_information& info, unsigned int depth) {

	}
}

