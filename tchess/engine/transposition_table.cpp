/*
 * transposition_table.cpp
 *
 *  Created on: 2021. febr. 9.
 *      Author: Gáspár Tamás
 */

#include "transposition_table.h"

namespace tchess
{
	uint64 createZobrishHash(const chessboard& board, const game_information& info) {
		uint64 boardKey = 0;
		unsigned int rank = 0, file = 0;
		//iterate board and find pieces
		for(unsigned int square = 0; square < 64; ++square) {
			if(board[square] != 0) { //piece found
				unsigned int polyPiece = polyPieceCode(board[square]); //get polyglot piece code
				//find rank and board of square
				file = square % 8;
				rank = 7 - square / 8;
				//"add" piece to polyglot key
				boardKey ^= Random64[(64 * polyPiece) + (8 * rank) + file];
			}
		}
		//add castling rights to board key
		int offset = 768;
		if(info.getKingsideCastleRights(white)) {
			boardKey ^= Random64[offset];
		}
		if(info.getQueensideCastleRights(white)) {
			boardKey ^= Random64[offset + 1];
		}
		if(info.getKingsideCastleRights(black)) {
			boardKey ^= Random64[offset + 2];
		}
		if(info.getQueensideCastleRights(black)) {
			boardKey ^= Random64[offset + 3];
		}
		//add en passant sqaure to board key
		offset = 772;
		int whiteEnPassantSquare = info.getEnPassantSquare(white);
		int blackEnPassantSquare = info.getEnPassantSquare(black);
		int enPassantSquare = noEnPassant;
		if(whiteEnPassantSquare != noEnPassant) { //there will only be at most one of ep squares
			enPassantSquare = whiteEnPassantSquare;
		} else if(blackEnPassantSquare != noEnPassant) {
			enPassantSquare = blackEnPassantSquare;
		}
		if(enPassantSquare != noEnPassant) {
			unsigned int enPassantFile = enPassantSquare % 8;
			boardKey ^= Random64[offset + enPassantFile];
		}
		//add side to move to key
		offset = 780;
		if(info.getSideToMove() == black) {
			boardKey ^= Random64[offset];
		}
		return boardKey;
	}

	const unsigned short exact = 1;
	const unsigned short upperBound = 2;
	const unsigned short lowerBound = 3;
	const unsigned short uninitialized = 4;

	const transposition_entry EMPTY_ENTRY = transposition_entry();

	const unsigned int def_transposition_table_size = 256000000 / sizeof(transposition_entry);
}



