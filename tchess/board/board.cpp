#include <stdexcept>
#include <cstring>
#include <iostream>
#include <cstdlib>

#include "board.h"

/*
 * board.cpp
 *
 *  Created on: 2021. jan. 24.
 *      Author: Gáspár Tamás
 */

//Macro that negates a piece code if we are on black side
#define OWN_SIDE(side, piece) ((side)==0 ? (piece) : -(piece))

//Macro that gives the value of the piece on the enemy side
#define ENEMY_SIDE(side, piece) ((side)==0 ? -(piece) : (piece))

namespace tchess
{
	//Identifier for the white player.
	extern const unsigned int white = 0;

	//Identifier for the black player.
	extern const unsigned int black = 1;

	//Constant that indicated no en passant capture is possible.
	extern const int noEnPassant = - 10;

	//Constant for the empty squares.
	extern const unsigned int empty = 0;

	//Constant for pawns (negated for black).
	extern const unsigned int pawn = 1;

	//Constant for knights (negated for black).
	extern const unsigned int knight = 2;

	//Constant for bishops (negated for black).
	extern const unsigned int bishop = 3;

	//Constant for rooks (negated for black).
	extern const unsigned int rook = 4;

	//Constant for kings (negated for black).
	extern const unsigned int king = 5;

	//Constant for queens (negated for black).
	extern const unsigned int queen = 6;

	//This is how the board looks at the start of a game
	extern const int def_squares[64] = {
		-4, -2, -3, -6, -5, -3, -2, -4,
		-1, -1, -1, -1, -1, -1, -1, -1,
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,
		 1,  1,  1,  1,  1,  1,  1,  1,
		 4,  2,  3,  6,  5,  3,  2,  4
	};

	//Mailbox array with sentinel squares (-1).
	extern const int mailbox[120] = {
		 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		 -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
		 -1,  8,  9, 10, 11, 12, 13, 14, 15, -1,
		 -1, 16, 17, 18, 19, 20, 21, 22, 23, -1,
		 -1, 24, 25, 26, 27, 28, 29, 30, 31, -1,
		 -1, 32, 33, 34, 35, 36, 37, 38, 39, -1,
		 -1, 40, 41, 42, 43, 44, 45, 46, 47, -1,
		 -1, 48, 49, 50, 51, 52, 53, 54, 55, -1,
		 -1, 56, 57, 58, 59, 60, 61, 62, 63, -1,
		 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	};

	//64 sized mailbox array.
	extern const int mailbox64[64] = {
		21, 22, 23, 24, 25, 26, 27, 28,
		31, 32, 33, 34, 35, 36, 37, 38,
		41, 42, 43, 44, 45, 46, 47, 48,
		51, 52, 53, 54, 55, 56, 57, 58,
		61, 62, 63, 64, 65, 66, 67, 68,
		71, 72, 73, 74, 75, 76, 77, 78,
		81, 82, 83, 84, 85, 86, 87, 88,
		91, 92, 93, 94, 95, 96, 97, 98
	};

	//determines if pieces can slide
	extern const bool canSlide[7] = {false, false, false, true, true, false, true};

	//determines the amount of directions pieces can move to
	extern const unsigned int offsetAmount[7] = {0, 0, 8, 4, 4, 8, 8};

	//the exact offsets to get the directions of each piece
	extern const int offsets[7][8] = {
			{   0,   0,  0,  0, 0,  0,  0,  0 }, // <-- unused
			{   0,   0,  0,  0, 0,  0,  0,  0 }, // <-- unused
			{ -21, -19,-12, -8, 8, 12, 19, 21 }, // <-- KNIGHT
			{ -11,  -9,  9, 11, 0,  0,  0,  0 }, // <-- BISHOP
			{ -10,  -1,  1, 10, 0,  0,  0,  0 }, // <-- ROOK
			{ -11, -10, -9, -1, 1,  9, 10, 11 }, // <-- KING
			{ -11, -10, -9, -1, 1,  9, 10, 11 }  // <-- QUEEN
		};

	static const char firstFile = 'a';

	//chessboard class implementations
	static unsigned int defaultKingSquares[2] = {60, 4};

	chessboard::chessboard() {
		//set board square as default
		std::memcpy(squares, tchess::def_squares, sizeof(squares));
		kingSquare[white] = defaultKingSquares[white]; //initial king positions
		kingSquare[black] = defaultKingSquares[black];
	}

	chessboard::chessboard(const int (&squares)[64]) {
		kingSquare[white] = 100;
		kingSquare[black] = 100;
		for(int i=0; i<64; ++i) {
			this->squares[i] = squares[i];
			if(squares[i] == 5) {
				kingSquare[white] = i;
			} else if(squares[i] == -5) {
				kingSquare[black] = i;
			}
		}
		if(kingSquare[white]==100 || kingSquare[black]==100) {
			throw std::runtime_error("2 kings not found on this board!");
		}
	}

	int chessboard::makeMove(const move& m, unsigned int side) {
		const unsigned int from = m.getFromSquare(), to = m.getToSquare();
		int captured = empty;
		if(m.isKingsideCastle()) { //kingside castle moves 2 pieces
			//the move's from and to squares are the kings position before and after the castle
			squares[from] = empty; //move king
			squares[to] = OWN_SIDE(side, 5);
			kingSquare[side] = to; //keep king location updated
			squares[to + 1] = empty; //move rook
			squares[from + 1] = OWN_SIDE(side, 4);
		} else if(m.isQueensideCastle()) { //queenside castle moves 2 pieces
			//the moves from and to squares are the kings position before and after the castle
			squares[from] = empty; //move king
			squares[to] = OWN_SIDE(side, 5);
			kingSquare[side] = to; //keep king location updated
			squares[to - 2] = empty; //move rook
			squares[from - 1] = OWN_SIDE(side, 4);
		} else if(m.isEnPassant()) { //en passant is special, because the captures piece is not on the destination square
			squares[from] = empty; //move pawn
			squares[to] = OWN_SIDE(side, 1);
			int capturedAt = side == white ? to+8 : to-8;
			squares[capturedAt] = empty; //remove pawn
			captured = ENEMY_SIDE(side, 1); //save that the captured piece was an enemy pawn
		} else if(m.isPromotion()) { //in case of promotion we need to look at what the pawn was promoted to
			int p = (int)m.promotedTo();
			int promotedTo = OWN_SIDE(side, p);
			captured = squares[to]; //if there was a capture, this will save it, if not nothing changes
			squares[from] = empty;
			squares[to] = promotedTo; //make the promoted piece
		} else { //nothing special about this move
			captured = squares[to]; //if there was a capture, this will save it, if not nothing changes
			int pieceThatMoved = squares[from];
			if(pieceThatMoved == 5 || pieceThatMoved == -5) { //using 5 instead of 'king' const to eliminate warning
				kingSquare[side] = to; //keep king location updated
			}
			squares[from] = empty;
			squares[to] = pieceThatMoved;
		}
		return captured;
	}

	void chessboard::unmakeMove(const move& m, unsigned int side, int capturedPiece) {
		const unsigned int from = m.getFromSquare(), to = m.getToSquare();
		if(m.isKingsideCastle()) { //kingside castle moved 2 pieces that need to be unmaked
			//the move's from and to squares are the kings position before and after the castle
			squares[to] = empty; //unmake king move
			squares[from] = OWN_SIDE(side, 5);
			kingSquare[side] = from; //keep king location updated
			squares[to + 1] = OWN_SIDE(side, 4); //unmake rook move
			squares[from + 1] = empty;
		} else if(m.isQueensideCastle()) { //queenside castle moves 2 pieces that need to unmaked
			//the moves from and to squares are the kings position before and after the castle
			squares[from] = OWN_SIDE(side, 5); //unmove king
			squares[to] = empty;
			kingSquare[side] = from; //keep king location updated
			squares[to - 2] = OWN_SIDE(side, 4); //unmove rook
			squares[from - 1] = empty;
		} else if(m.isEnPassant()) { //en passant is special, because the captures piece is not on the destination square
			squares[from] = OWN_SIDE(side, 1); //unmove own pawn
			squares[to] = empty;
			int capturedAt = side == white ? to+8 : to-8;
			squares[capturedAt] = ENEMY_SIDE(side, 1); //replace enemy pawn
		} else if(m.isPromotion()) { //in case of promotion a pawn must be placed to the departure square instead of the promotion piece
			squares[from] = OWN_SIDE(side, 1); //replace pawn
			squares[to] = capturedPiece; // replace the captured piece (sets to empty in case of no capture)
		} else  { //normal move
			int pieceThatMoved = squares[to]; //we can find the piece that moved on the destination square
			squares[from] = pieceThatMoved; //place to its original square
			if(pieceThatMoved == 5 || pieceThatMoved == -5) { //using 5 instead of 'king' const to eliminate warning
				kingSquare[side] = from; //keep king location updated
			}
			squares[to] = capturedPiece; // replace the captured piece (sets to empty in case of no capture)
		}
	}

	bool chessboard::isInsufficientMaterial() const {
		//count all pieces
		int pieceCounts[2][7] = {
				{0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0, 0, 0}
		};
		for(int square = 0; square < 64; ++square) {
			int piece = squares[square];
			if(piece != empty) {
				int side = piece > 0 ? white : black;
				piece = piece > 0 ? piece : -piece;
				pieceCounts[side][piece]++;
			}
		}
		//it can only be insufficient material if both sides does not have: pawn, rook, queen
		if(pieceCounts[white][pawn]==0 && pieceCounts[black][pawn]==0 &&
		   pieceCounts[white][rook]==0 && pieceCounts[black][rook]==0 &&
		   pieceCounts[white][queen]==0 && pieceCounts[black][queen]==0) {
			/*
			 * Both sides only have kings, knights, bishops:
			 * king and 0/1piece vs king and 0/1 piece is insufficient
			 */
			unsigned int whiteNonKing = pieceCounts[white][knight] + pieceCounts[white][bishop];
			unsigned int blackNonKing = pieceCounts[black][knight] + pieceCounts[black][bishop];
			if(whiteNonKing <= 1 && blackNonKing <= 1) {
				return true;
			}
		}
		return false;
	}

	std::string chessboard::to_string() const {
		std::string bS = "---------------------------\n";
		for(int i=0; i<8; ++i) { //add each rank
			bS += "|";
			for(int j=0; j<8; ++j) {
				std::string pieceString = " ";
				int piece = squares[8*i + j]; //can also be empty
				int pieceType = piece >= 0 ? piece : -piece;
				char pieceChar;
				if(pieceType == empty) {
					pieceChar = '.';
				} else {
					pieceChar = pieceNameFromCode(pieceType);
				}
				pieceString += piece >= 0 ? " " : "-"; //add - for black pieces
				pieceString += pieceChar;
				bS += pieceString; //add piece string
			}
			bS += " |\n";
		}
		bS.append("---------------------------\n");
		return bS;
	}

	//implement game_informatin class

	game_information::game_information() {
		kingsideCastleRight[white] = true;
		kingsideCastleRight[black] = true;
		queensideCastleRight[white] = true;
		queensideCastleRight[black] = true;
		enPassantCaptureSquares[white] = noEnPassant;
		enPassantCaptureSquares[black] = noEnPassant;
		sideToMove = white; //white begins
	}

	void game_information::disableKingsideCastleRight(unsigned int side) {
		kingsideCastleRight[side] = false;
	}

	void game_information::disableQueensideCastleRight(unsigned int side) {
		queensideCastleRight[side] = false;
	}

	void game_information::setEnPassantSquare(unsigned int side, unsigned int square) {
		enPassantCaptureSquares[side] = square;
	}

	//update method

	static unsigned int defaultRookSquares[2][2] = { //queenside first, then kingside
			{56, 63}, // <-- white rook squares
			{0, 7} // <-- black rook squares
	};

	void updateGameInformation(const chessboard& board, const move& m, game_information& info) {
		int sideThatMoved = info.getSideToMove(); //not updated yet
		int enemySide = 1-sideThatMoved;
		//handle castle rights
		int onKingsSquare = board[defaultKingSquares[sideThatMoved]];
		int onQRookSquare = board[defaultRookSquares[sideThatMoved][0]];
		int onKRookSquare = board[defaultRookSquares[sideThatMoved][1]];
		if(onKingsSquare != 5 && onKingsSquare != -5) { //if the king is not on his def square anymore, both catle rights are lost
			info.disableKingsideCastleRight(sideThatMoved);
			info.disableQueensideCastleRight(sideThatMoved);
		} else if(onQRookSquare != 4 && onQRookSquare != -4) { //if the queenside rook is not at it's place, then disable queenside castle right
			info.disableQueensideCastleRight(sideThatMoved);
		} else if(onKRookSquare != 4 && onKRookSquare != -4) {  //if the kingside rook is not at it's place, then disable kingside castle right
			info.disableKingsideCastleRight(sideThatMoved);
		}
		if(m.isKingsideCastle() || m.isQueensideCastle()) {
			info.setHasCastled(sideThatMoved);
		}
		//remove previous en passant squares, since they 'expire after 1 move'
		info.setEnPassantSquare(sideThatMoved, noEnPassant);
		info.setEnPassantSquare(enemySide, noEnPassant);
		//check for new en passant square
		if(m.isDoublePawnPush()) { //last move was a double pawn push
			unsigned int squareBehindPawn = sideThatMoved==white ? m.getToSquare()+8 : m.getToSquare()-8;
			//square behin pawn is a possible en passant square for the ENEMY side
			info.setEnPassantSquare(enemySide, squareBehindPawn);
		}

		info.setSideToMove(enemySide); //update side to move
	}

	//name / number conversion methods

	std::string createSquareName(int square) {
		std::string squareName;
		int rank = 8 - square / 8;
		char file = firstFile + square % 8;
		squareName += file;
		squareName += std::to_string(rank);
		return squareName;
	}

	unsigned int createSquareNumber(const std::string& squareName) {
		if(squareName.size() != 2) throw std::runtime_error("Invalid name!");
		char file = squareName[0];
		unsigned int rank = squareName[1] - '0';
		unsigned int number = ((8-rank) * 8) + (file - 'a');
		if(number < 0 || number > 63) {
			throw std::runtime_error("Invalid name!");
		}
		return number;
	}

	//move generation implementation

	void move_generator::generatePseudoLegalPawnMoves(unsigned int side, unsigned int square, std::vector<move>& moves) const {
		int inFront, captureLeft, captureRight;
		bool hasCaptureLeftSquare = false, hasCaptureRightSquare = false;
		if(side == white) { //pawn moves for white
			inFront = board[square-8]; //this square is "in front" of the black pawn
			if(square % 8 != 0) { //the pawn is not on the leftmost file, and can possibly capture left
				hasCaptureLeftSquare = true;
				captureLeft = board[square-9]; //find what is on the left capture square
			}
			if(square % 8 != 7) { //the pawn is not on the rightmost file and can possibly capture right
				hasCaptureRightSquare = true;
				captureRight = board[square-7]; //find what is on the right capture square
			}
			if(square / 8 == 6) { //the pawn is on its original rank, and may be able to make a double move
				int inFrontFar = board[square-16]; //this square is in front of the square in front of the white pawn
				//double pawn push is only possible of 2 squares in front of the pawn are empty
				if(inFront == empty && inFrontFar == empty) {
					moves.push_back(move(square, square-16, doublePawnPush, 0)); //register possible double pawn push
					//adding the en passant capture square is not done here
				}
				if(inFront == empty) { //the pawn can still choose to only move one square
					moves.push_back(move(square, square-8, quietMove, 0));
				}
				//despite the double push, the pawn can still choose to capture
				if(hasCaptureLeftSquare && captureLeft < 0) { //enemy piece on the left capture square
					int captured = std::abs(captureLeft);
					moves.push_back(move(square, square-9, capture, mvvLvaArray[pawn][captured]));
				}
				if(hasCaptureRightSquare && captureRight < 0) { //enemy piece on the right capture square
					int captured = std::abs(captureRight);
					moves.push_back(move(square, square-7, capture, mvvLvaArray[pawn][captured]));
				}
			} else if(square / 8 == 1) { //the pawn is ready to promote if it can move/capture forward
				if(inFront == empty) { //the pawn can quietly move into the promotion rank if there is nothing in front
					moves.push_back(move(square, square-8, knightPromotion, promotionScoreArray[knightPromotion]));
					moves.push_back(move(square, square-8, bishopPromotion, promotionScoreArray[bishopPromotion]));
					moves.push_back(move(square, square-8, rookPromotion, promotionScoreArray[rookPromotion]));
					moves.push_back(move(square, square-8, queenPromotion, promotionScoreArray[queenPromotion]));
				}
				if(hasCaptureLeftSquare && captureLeft < 0) { //enemy piece on the left (promotion) capture square
					int captured = std::abs(captureLeft);
					moves.push_back(move(square, square-9, knightPromotionCap, mvvLvaArray[pawn][captured]+promotionScoreArray[knightPromotion]));
					moves.push_back(move(square, square-9, bishopPromotionCap, mvvLvaArray[pawn][captured]+promotionScoreArray[bishopPromotion]));
					moves.push_back(move(square, square-9, rookPromotionCap, mvvLvaArray[pawn][captured]+promotionScoreArray[rookPromotion]));
					moves.push_back(move(square, square-9, queenPromotionCap, mvvLvaArray[pawn][captured]+promotionScoreArray[queenPromotion]));
				}
				if(hasCaptureRightSquare && captureRight < 0) { //enemy piece on the right (promotion) capture square
					int captured = std::abs(captureRight);
					moves.push_back(move(square, square-7, knightPromotionCap, mvvLvaArray[pawn][captured]+promotionScoreArray[knightPromotion]));
					moves.push_back(move(square, square-7, bishopPromotionCap, mvvLvaArray[pawn][captured]+promotionScoreArray[bishopPromotion]));
					moves.push_back(move(square, square-7, rookPromotionCap, mvvLvaArray[pawn][captured]+promotionScoreArray[rookPromotion]));
					moves.push_back(move(square, square-7, queenPromotionCap, mvvLvaArray[pawn][captured]+promotionScoreArray[queenPromotion]));
				}
			} else { //the pawn can neither double push nor promote
				if(inFront == empty) { //the pawn can advance if the way is empty
					moves.push_back(move(square, square-8, quietMove, 0));
				}
				//the pawn can also capture
				if(hasCaptureLeftSquare && captureLeft < 0) { //enemy piece on the left capture square
					int captured = std::abs(captureLeft);
					moves.push_back(move(square, square-9, capture, mvvLvaArray[pawn][captured]));
				} else if(hasCaptureLeftSquare && (square-9) == gameInfo.getEnPassantSquare(white)) { //en passant capture possible to the left
					moves.push_back(move(square, square-9, enPassantCapture, mvvLvaArray[pawn][pawn]));
				}
				if(hasCaptureRightSquare && captureRight < 0) { //enemy piece on the right capture square
					int captured = std::abs(captureRight);
					moves.push_back(move(square, square-7, capture, mvvLvaArray[pawn][captured]));
				} else if(hasCaptureRightSquare && (square-7) == gameInfo.getEnPassantSquare(white)) { //en passant capture possible to the right
					moves.push_back(move(square, square-7, enPassantCapture, mvvLvaArray[pawn][pawn]));
				}
			}
		} else { //pawn moves for black
			inFront = board[square+8]; //this square is "in front" of the white pawn
			if(square % 8 != 0) { //the pawn is not on the leftmost file, and can possibly capture left
				hasCaptureLeftSquare = true;
				captureLeft = board[square+7]; //find what is on the left capture square
			}
			if(square % 8 != 7) { //the pawn is not on the rightmost file and can possibly capture right
				hasCaptureRightSquare = true;
				captureRight = board[square+9]; //find what is on the right capture square
			}
			if(square / 8 == 1) { //the pawn is on its original rank, and may be able to make a double move
				int inFrontFar = board[square+16]; //this square is in front of the square in front of the black pawn
				//double pawn push is only possible of 2 squares in front of the pawn are empty
				if(inFront == empty && inFrontFar == empty) {
					moves.push_back(move(square, square+16, doublePawnPush, 0)); //register possible double pawn push
					//adding the en passant capture square is not done here
				}
				if(inFront == empty) { //the pawn can still choose to only move one square
					moves.push_back(move(square, square+8, quietMove, 0));
				}
				//despite the double push, the pawn can still choose to capture
				if(hasCaptureLeftSquare && captureLeft > 0) { //enemy piece on the left capture square
					int captured = std::abs(captureLeft);
					moves.push_back(move(square, square+7, capture, mvvLvaArray[pawn][captured]));
				}
				if(hasCaptureRightSquare && captureRight > 0) { //enemy piece on the right capture square
					int captured = std::abs(captureRight);
					moves.push_back(move(square, square+9, capture, mvvLvaArray[pawn][captured]));
				}
			} else if(square / 8 == 6) { //the pawn is ready to promote if it can move/capture forward
				if(inFront == empty) { //the pawn can quietly move into the promotion rank if there is nothing in front
					moves.push_back(move(square, square+8, knightPromotion, promotionScoreArray[knightPromotion]));
					moves.push_back(move(square, square+8, bishopPromotion, promotionScoreArray[bishopPromotion]));
					moves.push_back(move(square, square+8, rookPromotion, promotionScoreArray[rookPromotion]));
					moves.push_back(move(square, square+8, queenPromotion, promotionScoreArray[queenPromotion]));
				}
				if(hasCaptureLeftSquare && captureLeft > 0) { //enemy piece on the left (promotion) capture square
					int captured = std::abs(captureLeft);
					moves.push_back(move(square, square+7, knightPromotionCap, mvvLvaArray[pawn][captured]+promotionScoreArray[knightPromotion]));
					moves.push_back(move(square, square+7, bishopPromotionCap, mvvLvaArray[pawn][captured]+promotionScoreArray[bishopPromotion]));
					moves.push_back(move(square, square+7, rookPromotionCap, mvvLvaArray[pawn][captured]+promotionScoreArray[rookPromotion]));
					moves.push_back(move(square, square+7, queenPromotionCap, mvvLvaArray[pawn][captured]+promotionScoreArray[queenPromotion]));
				}
				if(hasCaptureRightSquare && captureRight > 0) { //enemy piece on the right (promotion) capture square
					int captured = std::abs(captureRight);
					moves.push_back(move(square, square+9, knightPromotionCap, mvvLvaArray[pawn][captured]+promotionScoreArray[knightPromotion]));
					moves.push_back(move(square, square+9, bishopPromotionCap, mvvLvaArray[pawn][captured]+promotionScoreArray[bishopPromotion]));
					moves.push_back(move(square, square+9, rookPromotionCap, mvvLvaArray[pawn][captured]+promotionScoreArray[rookPromotion]));
					moves.push_back(move(square, square+9, queenPromotionCap, mvvLvaArray[pawn][captured]+promotionScoreArray[queenPromotion]));
				}
			} else { //the pawn can neither double push nor promote
				if(inFront == empty) { //the pawn can advance if the way is empty
					moves.push_back(move(square, square+8, quietMove, 0));
				}
				//the pawn can also capture
				if(hasCaptureLeftSquare && captureLeft > 0) { //enemy piece on the left capture square
					int captured = std::abs(captureLeft);
					moves.push_back(move(square, square+7, capture, mvvLvaArray[pawn][captured]));
				}  else if(hasCaptureLeftSquare && (square+7) == gameInfo.getEnPassantSquare(black)) { //en passant capture possible to the left
					moves.push_back(move(square, square+7, enPassantCapture, mvvLvaArray[pawn][pawn]));
				}
				if(hasCaptureRightSquare && captureRight > 0) { //enemy piece on the right capture square
					int captured = std::abs(captureRight);
					moves.push_back(move(square, square+9, capture, mvvLvaArray[pawn][captured]));
				} else if(hasCaptureRightSquare && (square+9) == gameInfo.getEnPassantSquare(black)) { //en passant capture possible to the right
					moves.push_back(move(square, square+9, enPassantCapture, mvvLvaArray[pawn][pawn]));
				}
			}
		}
	}

	//Credits to chessprogramming.org for this useful code
	void move_generator::generatePseudoLegalNonPawnMoves(unsigned int side, unsigned int square, std::vector<move>& moves) const {
		/*
		 * Depending on the side, this may be negative, but now we wonly care about
		 * the type of the piece.
		 */
		const int piece = std::abs(board[square]);
		for(unsigned int i=0; i<offsetAmount[piece]; ++i) { //check all the directions (offsets) this piece can move to
			for (int n = square;;) {
				n = mailbox[mailbox64[n] + offsets[piece][i]]; //number of the next square in this direction
			    if (n == -1) break; //square is off the board
			    if (board[n] != empty) { //there is a piece on this square
			    	if ( (side==white && board[n] < 0) || (side == black && board[n] > 0) ) { //this piece is an enemy piece
			    		int captured = std::abs(board[n]);
			    		moves.push_back(move(square, n, capture, mvvLvaArray[piece][captured]));
			        }
			        break; //after capture, or if it was a friendly piece we can not move any more in this direction
			    }
			    //there was no piece here, can make a quiet move in this direction
			    moves.push_back(move(square, n, quietMove, 0));
			    if (!canSlide[piece]) break; //for non-sliding pieces, we must stop after 1 move in a direction
			}
		}
	}

	void move_generator::generatePseudoLegalNormalMoves(unsigned int side, std::vector<move>& moves) const {
		int kingCount = 0;
		if(side == white) { //generate moves for white
			for(unsigned int square = 0; square < 64; ++square) { //check all squares
				int piece = board[square];
				if(piece == 5 || piece == -5) ++kingCount;
				if(piece > 0 && piece < 7) { //found a white piece
					if(piece == pawn) { //look for white pawn moves
						generatePseudoLegalPawnMoves(side, square, moves); //find all moves for this white pawn
					} else { //look for white's non pawn moves
						generatePseudoLegalNonPawnMoves(side, square, moves); //find all moves for this white piece
					}
				}
			}
		} else { //generate moves for black
			for(unsigned int square = 0; square < 64; ++square) { //check all squares
				int piece = board[square];
				if(piece == 5 || piece == -5) ++kingCount;
				if(piece < 0) { //found a black piece
					if(piece == -1) { //look for black pawn moves (using -1 to eliminate warning)
						generatePseudoLegalPawnMoves(side, square, moves); //find all moves for black pawn
					} else { //look for black's non pawn moves
						generatePseudoLegalNonPawnMoves(side, square, moves); //find all moves for this black piece
					}
				}
			}
		}
		if(kingCount > 2) {
			throw std::runtime_error("Too many kings!");
		}
	}

	void move_generator::generatePseudoLegalCastleMoves(unsigned int side, std::vector<move>& moves) const {
		if(side == white) { //make castling moves for white
			if(gameInfo.getKingsideCastleRights(white)) { //look for kingside castle, if white still has the right
				if(board[60] == 5 && board[61] == empty && board[62] == empty && board[63] == 4) {
					moves.push_back(move(60,62,kingsideCastle, 0));
				}
			}
			if(gameInfo.getQueensideCastleRights(white)) { //look for kingside castle, if white still has the right
				if(board[56] == 4 && board[57] == empty &&
						board[58] == empty && board[59] == empty && board[60] == 5) {
					moves.push_back(move(60,58,queensideCastle, 0));
				}
			}
		} else { //make castling moves for black
			if(gameInfo.getKingsideCastleRights(black)) { //look for kingside castle, if black still has the right
				if(board[4] == -5 && board[5] == empty && board[6] == empty && board[7] == -4) {
					moves.push_back(move(4,6,kingsideCastle, 0));
				}
			}
			if(gameInfo.getQueensideCastleRights(black)) { //look for kingside castle, if black still has the right
				if(board[0] == -4 && board[1] == empty &&
						board[2] == empty && board[3] == empty && board[4] == -5) {
					moves.push_back(move(4,2,queensideCastle, 0));
				}
			}
		}
	}

	void move_generator::generatePseudoLegalMoves(unsigned int side, std::vector<move>& moves) const {
		moves.clear(); //empty list
		generatePseudoLegalNormalMoves(side, moves); //add normal moves
		generatePseudoLegalCastleMoves(side, moves); //add castling moves
	}

	static const int pawnAttackOffsets[2][2] = {
			{9, 11}, // <-- for white
			{-9, -11} // <-- for black
	};

	bool isAttacked(const chessboard& board, unsigned int side, unsigned int square) { //side is the attacker side
		bool attacked = false;
		/*
		 * To determine if the square is attacked I will look at all directions from that square. Can
		 * get all directions by looking at the QUEEN + KNIGHT offsets. The queen's offsets will be sliding,
		 * the knight's will not be.
		 */
		static int pieces[2] = {queen, knight};
		for(int p = 0; p < 2; ++p) {
			int piece = pieces[p];
			for(unsigned int i=0; i<offsetAmount[piece]; ++i) { //check all the directions (offsets) this piece can move to
				unsigned int distance = 0; //the "distance" we have moved on the current direction (also 1 for a knight move)
				int direction = offsets[piece][i]; //the direction where we are checking threats

				for (int n = square;;) {
					n = mailbox[mailbox64[n] + direction]; //number of the next square in this direction

					if (n == -1) break; //square is off the board
					//std::cout << "Next square in " << direction << " direction:" << createSquareName(n) << std::endl;
					++distance; //there is a square in this direction, so increase the distance

					if (board[n] != empty) { //there is a piece on this square
						if ( (side==white && board[n] > 0) || (side == black && board[n] < 0) ) { //this piece is an enemy piece (because 'side' is the attacker)

							//std::cout << "Enemy piece found in direction " << direction << ": " << board[n] << std::endl;
							/*
							 * There is a direction from the attacked square to an enemy piece. Depending on the distance and the
							 * type of the enemy piece, it may be able to attack this square. For example, if the direction is diagonal
							 * then a a bishop or a pawn can attack but a pawn only if the distance is 1 (and only from 2 diagonals).
							 */
							const unsigned int enemyPiece = board[n] >= 0 ? board[n] : -board[n]; //type of the enemy piece that may be attacking
							if(enemyPiece == pawn) { //pawns need special care, because they can only attack on 2 diagonals, depending on the side
								if(distance==1 && (direction == pawnAttackOffsets[side][0] || direction == pawnAttackOffsets[side][1])) {
									//the enemy pawn is 1 square away and on a diagonal from where it can attack
									//std::cout << "Attacking pawn found on square " << createSquareName(n) << " (distance " << distance << ")" << std::endl;
									return  true;
								}
							} else { //non pawn pieces
								if(canSlide[enemyPiece]) { //sliding piece, so ditance does not matter
									for(unsigned int k = 0; k<offsetAmount[enemyPiece]; ++k) { //check all direction the enemy piece can attack on
										if(direction == offsets[enemyPiece][k]) { //if the enemy sliding piece has the direction we are checking, then it can attack
											//std::cout << "Attacking sliding piece found" << std::endl;
											return true;
										}
									}
								} else { //this is a non sliding piece
									if(distance == 1) { //can only threaten if the distance is 1
										for(unsigned int k = 0; k<offsetAmount[enemyPiece]; ++k) { //check all direction the enemy piece can attack on
											if(direction == offsets[enemyPiece][k]) { //if the enemy piece has the direction we are checking, then it can attack
												//std::cout << "Attacking non-sliding piece found" << std::endl;
												return true;
											}
										}
									}
								}
							}
						}
						break; //any other pieces in this direction can't attack the square
					}
					if (!canSlide[piece]) break; //for non-sliding pieces, we must stop after 1 move in a direction
				}
			}
		}
		return attacked;
	}

	bool isLegalMove(const move& playerMove, chessboard& board, const game_information& info) {
		bool legal = false;
		unsigned int side = info.getSideToMove();
		unsigned int enemySide = 1 - side;
		int capturedPiece = board.makeMove(playerMove, side); //make this move on the board
		if(playerMove.isKingsideCastle()) {
			if(info.getKingsideCastleRights(side)) {
				//for castle, not only king safety needs to be checked, but also the king cant move through attacked squares
				if(!isAttacked(board, enemySide, playerMove.getFromSquare()) && // <-- check that king did not castle out of check
				   !isAttacked(board, enemySide, playerMove.getFromSquare()+1) && // <-- check that king did not castle through check
				   !isAttacked(board, enemySide, playerMove.getFromSquare()+2)) { // <-- check that king did not castle into check
					//the king is safe and did not catle out/through attacked squares
					legal = true;
				}
			}
		} else if(playerMove.isQueensideCastle()) {
			//for castle, not only king safety needs to be checked, but also the king cant move through attacked squares
			if(info.getQueensideCastleRights(side)) {
				if(!isAttacked(board, enemySide, playerMove.getFromSquare()) && // <-- check that king did not castle out of check
					   !isAttacked(board, enemySide, playerMove.getFromSquare()-1) && // <-- check that king did not castle through check
					   !isAttacked(board, enemySide, playerMove.getFromSquare()-2)) { // <-- check that king did not castle into check
					//the king is safe and did not catle out/through attacked squares
					legal = true;
				}
			}
		} else {
			//not a castle, only king safety is important
			if(!isAttacked(board, enemySide, board.getKingSquare(side))) {
				//the king is safe after this move
				legal = true;
			}
		}
		board.unmakeMove(playerMove, side, capturedPiece);
		return legal;
	}
}


