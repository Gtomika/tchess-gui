/*
 * move.h
 *
 * This header contains the move class that describes chess moves.
 *
 *  Created on: 2021. jan. 23.
 *      Author: Gáspár Tamás
 */

#ifndef SRC_BOARD_MOVE_H_
#define SRC_BOARD_MOVE_H_

#define NO_PROMOTION 'a'

#include <string>
#include <bitset>
#include <stdexcept>

namespace tchess
{
	//Special move that indicated that player resigned.
	extern const unsigned int resignMove;

	//Move code for a "non-special" move that results in no capture.
	extern const unsigned int quietMove;

	//Move code for double pawn pushes.
	extern const unsigned int doublePawnPush;

	//Move code for kingside castle.
	extern const unsigned int kingsideCastle;

	//Move code for queenside castle.
	extern const unsigned int queensideCastle;

	//Move code for captures.
	extern const unsigned int capture;

	//Move code for en-passant captures.
	extern const unsigned int enPassantCapture;

	//Move code for a pawn push that results in a knight promotion.
	extern const unsigned int knightPromotion;

	//Move code for a pawn push that results in a bishop promotion.
	extern const unsigned int bishopPromotion;

	//Move code for a pawn push that results in a rook promotion.
	extern const unsigned int rookPromotion;

	//Move code for a pawn push that results in a queen promotion.
	extern const unsigned int queenPromotion;

	//Move code for a pawn push that captures and results in a knight promotion.
	extern const unsigned int knightPromotionCap;

	//Move code for a pawn push that captures and results in a bishop promotion.
	extern const unsigned int bishopPromotionCap;

	//Move code for a pawn push that captures and results in a rook promotion.
	extern const unsigned int rookPromotionCap;

	//Move code for a pawn push that captures and results in a queen promotion.
	extern const unsigned int queenPromotionCap;

	/*
	 * This exception is thrown when a move string could not
	 * be parsed into a move.
	 */
	class move_parse_exception: public std::exception {
		std::string message;
	public:
		move_parse_exception(char const* const message) throw() : message(message) {}
		virtual char const* what() const throw();
	};

	/*
	 * Stores the result of a move legality test. Used to return not only the boolean value, but
	 * the reason for illegality as well.
	 */
	class move_legality_result {

		const bool legal;

		const bool pseudoLegal;

		std::string information;

		//Needed to unmake move
		const int capturedPiece;

	public:
		move_legality_result(bool legal, bool pl, const std::string& info, int cp)
			: legal(legal), pseudoLegal(pl), information(info), capturedPiece(cp) {}

		inline bool isLegal() const {
			return legal;
		}

		inline bool isPseudoLegal() const {
			return pseudoLegal;
		}

		inline std::string getInformation() const {
			return information;
		}

		inline int getCapturedPiece() const {
			return capturedPiece;
		}
	};

	/*
	 * This array stores what score a move will get depending on what piece captures
	 * what.
	 */
	extern const unsigned int mvvLvaArray[7][7];

	/*
	 * Contains move scores for promotion types.
	 */
	extern const unsigned int promotionScoreArray[16];

	/**
	 * Represents a chess move, using the departure square (from) and the destination
	 * square (to).
	 *
	 * Also includes 4 flags in a bitfield. These flags are:
	 *  - Bit 1: capture flag
	 *  - Bit 2: promotion flag
	 *  - Bit 3: Special flag 1
	 *  - Bit 4: Special flag 2
	 *
	 * The possible bitfields are declared as constants in this file. For example, bishop promotion capture
	 * is 13 (binary 1101), so both capture and promotion flags are 1, but only the second special flag is 1.
	 *
	 * For castling the departure and destination squares of the move will be the departure and destination
	 * of the king.
	 */
	class move {

		//Departure square of the move.
		unsigned int fromSquare;

		//Destination square of the move.
		unsigned int toSquare;

		//The 4 bit wide bitfield containing the flags.
		std::bitset<4> flags;

		/*
		 * The score of the move determines how high it will go in the move ordering. Player
		 * agents that do not care about move ordering can set this to 0.
		 * During move generation each generated move will get a score depending on the MvvLva scheme
		 * (most valuable victim, least valuable attacker).
		 */
		unsigned int score;

	public:
		move() : fromSquare(0), toSquare(0), score(0) {} //<- basically NULLMOVE

		//Create a move object
		move(unsigned int fromSquare, unsigned int toSquare, unsigned int flags, unsigned int score) :
			fromSquare(fromSquare), toSquare(toSquare), flags(flags), score(score) {}

		move(const move& other) : fromSquare(other.fromSquare), toSquare(other.toSquare),
				flags(0), score(other.score) {
			for(int i=0; i<4; ++i) {
				flags[i] = other.flags[i];
			}
		}

		/*
		 * Assignment operator.
		 */
		move& operator=(const move& other);

		//Checks if this move is the special resign move.
		inline bool isResign() const {
			return !flags[0] && flags[1] && flags[2] && !flags[3];
		}

		//Checks if this move is a capture.
		inline bool isCapture() const {
			//need to check the second bit (bitset goes backwards...)
			return flags[2];
		}

		//Checks if this move is a promotion.
		inline bool isPromotion() const {
			//need to check first bit  (bitset goes backwards...)
			return flags[3];
		}

		//Checks if this move is a double pawn push.
		inline bool isDoublePawnPush() const {
			//not capture, promotion and only second special bit is on  (bitset goes backwards...)
			return !flags[3] && !flags[2] && !flags[1] && flags[0];
		}

		//Checks if this move is a kingside castle.
		inline bool isKingsideCastle() const {
			//not capture, promotion and only the first special bit is true
			return !flags[3] && !flags[2] && flags[1] && !flags[0];
		}

		//Checks if this move is a queenside castle.
		inline bool isQueensideCastle() const {
			//not capture, promotion and both special bits are true
			return !flags[3] && !flags[2] && flags[1] && flags[0];
		}

		//Checks if this move is an en-passant capture.
		inline bool isEnPassant() const {
			//is a capture but not a promotion and only second special bit is true
			return !flags[3] && flags[2] && !flags[1] && flags[0];
		}

		/*
		 * Returns the piece code of the piece the paawn was promoted into.
		 * This should only be called if the move is a promotion. Piece codes are
		 * found in board.h.
		 *
		 * This only returns the type of piece (only positive). For black, this should be negated.
		 */
		unsigned int promotedTo() const;

		inline unsigned int getFromSquare() const { return fromSquare; }

		inline unsigned int getToSquare() const { return toSquare; }

		inline unsigned int getScore() const { return score; }

		/*
		 * Equality check between 2 moves. They are equal if both the departure
		 * and the destination squares are equal. In case of promotions, the equality
		 * of the promoted piece is also checked.
		 */
		bool operator==(const move& other) const;

		/*
		 * Used to sort move vectors into descending order.
		 */
		 bool operator>(const move& m) const;

		/*
		 * Creates a string format of the move. Since the move
		 * object does not store what piece moved that must be passed in
		 */
		std::string to_string(int pieceThatMoved) const;
	};

	/*
	 * Move from square 0 to 0, can be used to indicate that no move was found/no move exists.
	 */
	extern const move NULLMOVE;

	//Helper method to convert piece codes into letters
	char pieceNameFromCode(int pieceCode);

	/*
	* Checks if a move is promotion.
	*/
	bool checkForPromotion(const std::string& fromSquareName, const std::string& toSquareName, unsigned int side);

	/*
	 * Make a move from squares and piece types.
	 */
	move parse_move(char pieceCode, unsigned int from, unsigned int to, unsigned int side, char promotionCode);
}

#endif /* SRC_BOARD_MOVE_H_ */
