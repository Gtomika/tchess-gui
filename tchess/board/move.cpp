#include <vector>
#include <sstream>
#include <iterator>

#include "move.h"
#include "board.h"
#include "../game/game.h"

/*
 * move.cpp
 *
 * Implementations for move.h
 *
 *  Created on: 2021. jan. 23.
 *      Author: Gáspár Tamás
 */

namespace tchess
{
	const unsigned int resignMove = 6;

	//Move code for a "non-special" move that results in no capture.
	extern const unsigned int quietMove = 0;

	//Move code for double pawn pushes.
	extern const unsigned int doublePawnPush = 1;

	//Move code for kingside castle.
	extern const unsigned int kingsideCastle = 2;

	//Move code for queenside castle.
	extern const unsigned int queensideCastle = 3;

	//Move code for captures.
	extern const unsigned int capture = 4;

	//Move code for en-passant captures.
	extern const unsigned int enPassantCapture = 5;

	//Move code for a pawn push that results in a knight promotion.
	extern const unsigned int knightPromotion = 8;

	//Move code for a pawn push that results in a bishop promotion.
	extern const unsigned int bishopPromotion = 9;

	//Move code for a pawn push that results in a rook promotion.
	extern const unsigned int rookPromotion = 10;

	//Move code for a pawn push that results in a queen promotion.
	extern const unsigned int queenPromotion = 11;

	//Move code for a pawn push that captures and results in a knight promotion.
	extern const unsigned int knightPromotionCap = 12;

	//Move code for a pawn push that captures and results in a bishop promotion.
	extern const unsigned int bishopPromotionCap = 13;

	//Move code for a pawn push that captures and results in a rook promotion.
	extern const unsigned int rookPromotionCap = 14;

	//Move code for a pawn push that captures and results in a queen promotion.
	extern const unsigned int queenPromotionCap = 15;

	/*
	 * Meaning of a row: capturing piece.
	 * Meaining of a column: captured piece.
	 * So for example 1,6 means pawn capturing queen which gets the highest score.
	 * King "capture" is also in but it is unused.
	 */
	const unsigned int mvvLvaArray[7][7] = {
			{0,      0,       0,       0,        0,        0,         0}, // <- unused, no piece at 0
			{0,     105,     205,     305,      405,       0,        505}, // <- pawn captures
			{0,     104,     204,     304,      404,       0,        504}, // <- knight captures
			{0,     103,     203,     303,      403,       0,        503}, // <- bishop captures
			{0,     102,     202,     302,      402,       0,        502}, // <- rook captures
			{0,     100,     200,     300,      400,       0,        500}, // <- king captures
			{0,     101,     201,     301,      401,       0,        501} // <- queen captures
		/*unused    pawn   knight    bishop    rook      king       queen */
	};

	const unsigned int promotionScoreArray[16] = {
			0, 0, 0, 0, 0, 0, 0, 0, 320, 330, 400, 900, 320, 330, 400, 900
	};

	unsigned int move::promotedTo() const {
		//assumes this a promotion, so only checking special bits
		int s1 = flags[1], s2 = flags[0];
		if(s1==1 && s2==1) return queen;
		if(s1==1 && s2==0) return rook;
		if(s1==0 && s2==1) return bishop;
		return knight;
	}

	bool move::operator==(const move& other) const {
		if(isPromotion()) {
			return fromSquare == other.fromSquare && toSquare == other.toSquare &&
					promotedTo() == other.promotedTo();
		} else { //the kings dep. and dest. squares are enough to check castle equality as well
			bool squaresEqual = fromSquare == other.fromSquare && toSquare == other.toSquare;
			bool flagsEqual = true;
			for(int i=0; i<4; ++i) {
				if(flags[i] != other.flags[i]) flagsEqual = false;
			}
			return squaresEqual && flagsEqual;
		}
	}

	 bool move::operator>(const move& m) const {
		 return score > m.score;
	 }

	move& move::operator=(const move& other) {
		fromSquare = other.fromSquare;
		toSquare = other.toSquare;
		score = other.score;
		for(int i=0; i<4; ++i) {
			flags[i] = other.flags[i];
		}
		return *this;
	}

	//Helper method to convert piece codes into letters
	char pieceNameFromCode(int pieceCode) {
		switch(pieceCode) {
		case 1:
			return 'P';
			break;
		case 2:
			return 'N';
			break;
		case 3:
			return 'B';
			break;
		case 4:
			return 'R';
			break;
		case 5:
			return 'K';
			break;
		case 6:
			return 'Q';
			break;
		default:
			return '?';
		}
	}

	std::string move::to_string(int pieceThatMoved) const {
		if(*this == NULLMOVE) {
			return "Null move";
		}
		if(isKingsideCastle()) { //special moves
			return "o-o";
		} else if(isQueensideCastle()) {
			return "o-o-o";
		}
		std::string moveString;
		moveString += pieceNameFromCode(pieceThatMoved);
		moveString += " ";
		moveString += createSquareName(fromSquare);
		if(isCapture()) { //write x if capture
			moveString += " x ";
		} else {
			moveString += " ";
		}
		moveString += createSquareName(toSquare);
		if(isPromotion()) { //write what it was promoted to if it was promotion
			moveString += " = ";
			int prom = promotedTo();
			moveString += pieceNameFromCode(prom);
		}
		if(isEnPassant()) { //indicate en passant
			moveString += " (ep)";
		}
		return moveString;
	}

	char const * move_parse_exception::what() const throw() {
		return message.c_str();
	}

	//Helper method that detects if the move is a promotion (assumes pawn move, correct square names)
	bool checkForPromotion(const std::string& fromSquareName, const std::string& toSquareName, unsigned int side) {
		if(side == white) {
			//white promoted if the move is from the 7. rank to the 8. rank
			return fromSquareName[1] == '7' && toSquareName[1] == '8';
		} else { //black
			//black promoted if the move is from the 2. rank to the 1. rank
			return fromSquareName[1] == '2' && toSquareName[1] == '1';
		}
	}

	const move NULLMOVE = move(0, 0, quietMove, 0);

	//move string will come in the form of: piece_code from_square to_square 
	move parse_move(char pieceCode, unsigned int fromSquare, unsigned int toSquare, unsigned int side, char promotionCode) {
		if (pieceCode != 'K' && pieceCode != 'Q' && pieceCode != 'P' && pieceCode != 'R' && pieceCode != 'N' && pieceCode != 'B') {
			throw move_parse_exception("No piece on the departure square!");
		}
		unsigned int ksCastleFrom = side == white ? 60 : 4;
		unsigned int ksCastleTo = side == white ? 62 : 6;
		unsigned int qsCastleFrom = side == white ? 60 : 4;
		unsigned int qsCastleTo = side == white ? 58 : 2;
		if(pieceCode == 'K' && fromSquare == ksCastleFrom && toSquare == ksCastleTo) {
			//this is a kingside castle
			return move(fromSquare, toSquare, kingsideCastle, 0);
		} else if(pieceCode == 'K' && fromSquare == qsCastleFrom && toSquare == qsCastleTo) {
			//this is a queenside castle
			return move(fromSquare, toSquare, queensideCastle, 0);
		}
		//if the move is a promotion
		if (pieceCode == 'P' && checkForPromotion(createSquareName(fromSquare), createSquareName(toSquare), side)) {
			//must have a promotion piece code in this case
			if (promotionCode == NO_PROMOTION) {
				throw move_parse_exception("This move appears to be a promotion, but no promotion piece code was found!");
			}
			unsigned int promMoveType;
			if (promotionCode == 'Q') {
				promMoveType = queenPromotion;
			}
			else if (promotionCode == 'R') {
				promMoveType = rookPromotion;
			}
			else if (promotionCode == 'N') {
				promMoveType = knightPromotion;
			}
			else if (promotionCode == 'B') {
				promMoveType = bishopPromotion;
			}
			else {
				throw move_parse_exception("Unrecognized promotion piece code!");
			}
			//move is a promotion, with a valid promotion piece code
			return move(fromSquare, toSquare, promMoveType, 0);
		}
		else { //this move does not appear to be a promotion
			unsigned int moveType = quietMove;
			//check for double pawn push
			if (pieceCode == 'P' && (toSquare - fromSquare == 16 || toSquare - fromSquare == -16)) {
				moveType = doublePawnPush;
			}
			/*
			 * This move may be a capture or en passant
			 * but we have no way of knowing that here, so just using quiet move type.
			 * It will be fixed in the 'captureFix' method of game controller
			 */
			return move(fromSquare, toSquare, moveType, 0);
		}
	}

	template <typename Out>
	void split(const std::string& s, char delim, Out result) {
		std::istringstream iss(s);
		std::string item;
		while (std::getline(iss, item, delim)) {
			*result++ = item;
		}
	}

	std::vector<std::string> split(const std::string& s, char delim) {
		std::vector<std::string> elems;
		split(s, delim, std::back_inserter(elems));
		return elems;
	}

	move parse_move(const std::string& moveString, unsigned int side) {
		int fromSquare, toSquare;
		if (moveString == "o-o") {
			//get kingside castle squares for KING, depending on side
			fromSquare = side == white ? 60 : 4;
			toSquare = side == white ? 62 : 6;
			return move(fromSquare, toSquare, kingsideCastle, 0);
		}
		else if (moveString == "o-o-o") {
			//get queenside castle squares for KING, depending on side
			fromSquare = side == white ? 60 : 4;
			toSquare = side == white ? 58 : 2;
			return move(fromSquare, toSquare, queensideCastle, 0);
		}
		//move is not castle, so there must be piece code, departure and destination squares specified
		std::vector<std::string> splitMove = split(moveString, ' ');
		int splitSize = splitMove.size();
		if (splitSize < 3) {
			throw move_parse_exception("Not enough move arguments!");
		}
		//the first token is the moved piece code
		std::string p = splitMove[0];
		if (p != "K" && p != "Q" && p != "P" && p != "R" && p != "N" && p != "B") {
			throw move_parse_exception("Unrecognized piece code!");
		}
		//the second + third argument should be the departure/destination squares
		try {
			fromSquare = createSquareNumber(splitMove[1]);
			toSquare = createSquareNumber(splitMove[2]);
			//if the move is a promotion
			if (p == "P" && checkForPromotion(splitMove[1], splitMove[2], side)) {
				//must have a promotion piece code as well in this case
				if (splitSize < 4) {
					throw move_parse_exception("This move appears to be a promotion, but no promotion piece code was found!");
				}
				std::string pProm = splitMove[3];
				unsigned int promMoveType;
				if (pProm == "Q") {
					promMoveType = queenPromotion;
				}
				else if (pProm == "R") {
					promMoveType = rookPromotion;
				}
				else if (pProm == "N") {
					promMoveType = knightPromotion;
				}
				else if (pProm == "B") {
					promMoveType = bishopPromotion;
				}
				else {
					throw move_parse_exception("Unrecognized promotion piece code!");
				}
				//move is a promotion, with a valid promotion piece code
				return move(fromSquare, toSquare, promMoveType, 0);
			}
			else { //this move does not appear to be a promotion
				unsigned int moveType = quietMove;
				if (splitMove[0] == "P" && (toSquare - fromSquare == 16 || toSquare - fromSquare == -16)) {
					moveType = doublePawnPush;
				}
				else if (splitMove[0] == "P" && splitMove.size() == 4 && splitMove[3] == "(ep)") {
					//appears to be an en passant
					moveType = enPassantCapture;
				}
				/*
				 * This move may be a capture
				 * but we have no way of knowing that here, so just using quiet move type.
				 */
				return move(fromSquare, toSquare, moveType, 0);
			}
		}
		catch (std::runtime_error&) { //failed to parse square names
			throw move_parse_exception("Unrecognized square name/names!");
		}
	}
}
