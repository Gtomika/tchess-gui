/*
 * board.h
 *
 * Chessboard representation.
 *  - Square centric.
 *  - Mailbox: white pices are positive, black pieces are negative, empty square is 0, the sentinel
 *  files and ranks are 7.
 *
 * The board is a 8x8 array looks like this:
 *  -4 -2 -3 -5 -6 -3 -2 -4  <-- black's backrank
 *  -1 -1 -1 -1 -1 -1 -1 -1  <-- black's pawn rank
 *   0  0  0  0  0  0  0  0
 *   0  0  0  0  0  0  0  0
 *   0  0  0  0  0  0  0  0
 *   0  0  0  0  0  0  0  0
 *   1  1  1  1  1  1  1  1  <-- white's pawn rank
 *   4  2  3  5  6  3  2  4  <-- white's backrank
 *
 *  Created on: 2021. jan. 23.
 *      Author: Gáspár Tamás
 */

#ifndef SRC_BOARD_BOARD_H_
#define SRC_BOARD_BOARD_H_

#include <string>
#include <vector>
#include <utility>

#include "move.h"

namespace tchess
{
	//Identifier for the white player.
	extern const unsigned int white;

	//Identifier for the black player.
	extern const unsigned int black;

	//Constant that indicated no en passant capture is possible.
	extern const int noEnPassant;

	//Constant for the empty squares.
	extern const unsigned int empty;

	//Constant for pawns (negated for black).
	extern const unsigned int pawn;

	//Constant for knights (negated for black).
	extern const unsigned int knight;

	//Constant for bishops (negated for black).
	extern const unsigned int bishop;

	//Constant for rooks (negated for black).
	extern const unsigned int rook;

	//Constant for kings (negated for black).
	extern const unsigned int king;

	//Constant for queens (negated for black).
	extern const unsigned int queen;

	//Default squares of the board, when starting a game.
	extern const int def_squares[64];

	/*
	 * 10x12 mailbox array, used to check if a move is out of bounds together with
	 * mailbox64 array.
	 */
	extern const int mailbox[120];

	/**
	 * 8x8 mailbox array, used to check if a move is out of bounds together with
	 * the 10x12 mailbox array.
	 */
	extern const int mailbox64[64];

	/**
	 * This array is used to determing if a piece can slide (using the piece codes). For example the
	 * rook (code 4) can slide, and so the fourth element in this array (index 3) is true.
	 */
	extern const bool canSlide[7];

	/**
	 * This array determines to how many directions (not literally in case of knights) a piece can move. For example the
	 * bishop (piece code 3) can move on the 4 diagonals, so the third element of this array is 4.
	 */
	extern const unsigned int offsetAmount[7];

	/**
	 * This array stores the move offsets for each piece. The offsets of piece p will be in offsets[p] sub array. For
	 * example the knights (code 2) offsets will be in offsets[2].
	 *
	 * In case of pieces where only 4 offset directions are possible the last 4 offsets will be 0 (and are unused).
	 *
	 * The meaning of this is that for example if the rook moves "down" by one square then we can get the destination
	 * by adding 10 to its current square, and so 10 is one offset of the rook.
	 */
	extern const int offsets[7][8];

	/*
	 * The chessboard class which stores the current board, and allows for making and
	 * unmaking moves. This class won't check for the validity of the moves, instead it assumes the
	 * moves received are at least pseudo legal.
	 */
	class chessboard {

		//Squares of the board
		int squares[64];

		/*
		 * Maintains the location of the king for both sides.
		 */
		unsigned int kingSquare[2];

	public:
		//Creates a chessboard as it is at the start of the game.
		chessboard();

		//Creates a chessboard from an array of squares and piece. Only for debug.
		chessboard(const int (&squares)[64]);

		/*
		 * This operator is used to access the squares of the board.
		 */
		inline int operator[](unsigned int i) const {
			return squares[i];
		}

		/*
		 * Modifies the chessboard according to the given move. Returns the code of the
		 * captured piece (or 'empty' if there was no capture). This return value can later
		 * be used by the 'unmakeMove' method to restore the board's previous state.
		 */
		int makeMove(const move&, unsigned int side);

		/*
		 * Unmakes the modifications made by the given move. This requires what the captured piece
		 * was, becase the move object does not store this information.
		 */
		void unmakeMove(const move&, unsigned int side, int capturedPiece);

		/*
		 * This method checks if there is a sufficient amount of material
		 * left on the board to continue. For example, if there are only 2 kings
		 * on the board then there can be no checkmate, so it is insufficient.
		 */
		bool isInsufficientMaterial() const;

		/*
		 * Mostly a debug method that will create a string representation of the board that can
		 * be printed to see if the move making/unmaking was correct. It uses the letters of each
		 * piece, and 0 for empty squares. Black pieces have a - sign in front of them.
		 *
		 * For example:
		 * ---------------------------
		 * | -R -N -B -Q -K -B -N -R |
		 * | -P -P -P -P -P -P -P -P |
		 * |  0  0  0  0  0  0  0  0 |
		 * |  0  0  0  0  0  0  0  0 |
		 * |  0  0  0  0  0  0  0  0 |
		 * |  0  0  0  0  0  0  0  0 |
		 * |  P  P  P  P  P  P  P  P |
		 * |  R  N  B  Q  K  B  N  R |
		 * ---------------------------
		 */
		std::string to_string() const;

		/*
		 * Can always quickly get the kings location for both sides.
		 */
		inline unsigned int getKingSquare(unsigned int side) const {
			return kingSquare[side];
		}
	};

	/*
	 * This class stores information about a chess game. Every kind of information
	 * is stored in pairs where the first part of the pair is for white, the second
	 * is for black.
	 */
	class game_information {

		/**
		 * Stores which side must make the next move. IS either the white or the black constant.
		 */
		unsigned int sideToMove;

		/*
		 * Stores if the players have right to castle kingside.
		 * NOTE: this does not mean that the player can castle in the next move, since they might have pieces between the
		 * king and the rook, or the king may be in check. This only means that a side has or doesn't have the right to castle,
		 * if possible.
		 */
		bool kingsideCastleRight[2];

		/**
		 * Stores if the players have right to castle queenside.
		 * NOTE: this does not mean that the player can castle in the next move, since they might have pieces between the
		 * king and the rook, or the king may be in check. This only means that a side has or doesn't have the right to castle,
		 * if possible.
		 */
		bool queensideCastleRight[2];

		//Stores if the sides have castled.
		bool hasCastled[2];

		/**
		 * Stores if the players can perform en passant captures in the next move, and at which squares.
		 * If no such capture is possible, then the value is the 'noEnPassant' constant.
		 */
		int enPassantCaptureSquares[2];

	public:

		/**
		 * Creates an information object where all properties are set as they are
		 * in the beginning of a game.
		 */
		game_information();

		/**
		 * This method stores that a side is no longer eligible for kingside castling. For example, if a
		 * side has moved their king then his will be called.
		 * Since castling right can't be gained back once lost, this method can only disable them.
		 * Side must be either the white or the black constant.
		 */
		void disableKingsideCastleRight(unsigned int side);

		/**
		* This method stores that a side is no longer eligible for queenside castling. For example, if a
		* side has moved their king then his will be called.
		* Since castling right can't be gained back once lost, this method can only disable them.
		* Side must be either the white or the black constant.
		*/
		void disableQueensideCastleRight(unsigned int side);

		/**
		 * Marks a square as a possible en passant capture square for a side.
		 * Side must be either the white or the black constant.
		 * To indicate that an en passant is no longer possible, use the 'noEnPassant' constant.
		 */
		void setEnPassantSquare(unsigned int side, unsigned int square);

		/*
		 * Getter for en passant square property.
		 */
		inline unsigned int getEnPassantSquare(unsigned int side) const {
			return enPassantCaptureSquares[side];
		}

		/*
		 * Getter for kingside castle rights.
		 */
		inline bool getKingsideCastleRights(unsigned int side) const {
			return kingsideCastleRight[side];
		}

		/*
		 * Getter for queenside castle rights.
		 */
		inline bool getQueensideCastleRights(unsigned int side) const {
			return queensideCastleRight[side];
		}

		inline void setHasCastled(unsigned int side) {
			hasCastled[side] = true;
		}

		inline bool getHasCastled(unsigned int side) const {
			return hasCastled[side];
		}

		/**
		 * Update the side to move variable.
		 */
		inline void setSideToMove(unsigned int side) {
			sideToMove = side;
		}

		/*
		 * Getter for the side to move variable.
		 */
		inline unsigned int getSideToMove() const {
			return sideToMove;
		}
	};

	/*
	 * This method will modify the 'info' object after a move. For example if a side has moved their king
	 * then they will loose castle rights.
	 * Includes:
	 *  - Update side to move to the next side.
	 *  - Update castling rights.
	 *  - Update en passant attack squares.
	 */
	void updateGameInformation(const chessboard& board, const move& m, game_information& info);

	/*
	 * This class creates moves from a chessboard and the side to move.
	 */
	class move_generator {

		/*
		 * Stores the current chessboard.
		 */
		const chessboard& board;

		/**
		 * Additional information about the game, that is needed to
		 * generate moves.
		 */
		const game_information& gameInfo;

	public:
		move_generator() = delete;

		move_generator(const chessboard& b, const game_information& gi) : board(b), gameInfo(gi) {}

		/*
		 * Generates the pseudo legal moves for a chessboard and the side to move. The list
		 * received as parameter will be cleared before, then filled with the moves. This method first
		 * generates moves using one piece, then castling moves.
		 *
		 * Pseudo legal moves are where it isn't checked if they leave their king in check.
		 */
		void generatePseudoLegalMoves(unsigned int side, std::vector<move>&) const;

	private:

		/*
		 * This method will generate all pseudo legal moves for 1 pawn, on the given square.
		 * It is checked by the called of this method that there is a pawn on this square.
		 */
		void generatePseudoLegalPawnMoves(unsigned int side, unsigned int square, std::vector<move>&) const;

		/**
		 * Generates all pseudo legal moves for a non pawn piece. IT is checked by the called of
		 * this method that there is a non pawn piece on this square
		 */
		void generatePseudoLegalNonPawnMoves(unsigned int side, unsigned int square, std::vector<move>&) const;

		/**
		 * Generates all pawn moves, including en passant captures and promotions.
		 */
		void generatePseudoLegalNormalMoves(unsigned int side, std::vector<move>&) const;

		/**
		 * Generates kingside and queenside castling moves. Pseudo legal means here that it it
		 * not checked whether the king is in check when castling or if it passes through attacked fields.
		 */
		void generatePseudoLegalCastleMoves(unsigned int side, std::vector<move>&) const;
	};

	/*
	 * Checks if the given square is under attack by the attack side.
	 */
	bool isAttacked(const chessboard& board, unsigned int attackingSide, unsigned int square);

	/**
	 * This method checks if a move is legal. It will take into consideration the board and
	 * the game_information object. It is assumed that the move is at least PSEUDO LEGAL!!!
	 *
	 * The game class has a similar method, but that one returns additional information as well, this is faster
	 * and used by the player agents who dont need information about why a move is not legal.
	 */
	bool isLegalMove(const move& m, chessboard& board, const game_information& info);

	/*
	 * Will create a readable representation of a field, such as b5. For example,
	 * from field 32 it will create a4.
	 */
	std::string createSquareName(int square);

	/*
	 * Inverse of the method above, it will determine the mailbox position of
	 * a square from its name. For example for "h1" it will return 63.
	 */
	unsigned int createSquareNumber(const std::string& squareName);
}

#endif /* SRC_BOARD_BOARD_H_ */
