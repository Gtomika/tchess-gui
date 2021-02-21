/*
 * evaluation.cpp
 *
 *	Many of the evaluation values/tables are from the 'simplified fevaluation function'.
 *
 *  Created on: 2021. febr. 1.
 *      Author: Gáspár Tamás
 */
#include <algorithm>

#include "evaluation.h"
#include "board/board.h"

namespace tchess
{
	const int pieceValues[7] = {0, 100, 320, 330, 500, 20000, 900};

	//piece square tables (for both sides)
	extern const int pawnTable[2][64] = {
	   {
			0,  0,  0,  0,  0,  0,  0,  0,
			50, 50, 50, 50, 50, 50, 50, 50,
			10, 10, 20, 30, 30, 20, 10, 10,
			5,  5, 10, 25, 25, 10,  5,  5,
			0,  0,  0, 20, 20,  0,  0,  0,
			5, -5,-10,  0,  0,-10, -5,  5,
			5, 10, 10,-20,-20, 10, 10,  5,
			0,  0,  0,  0,  0,  0,  0,  0
	   },
	   {
		    0,  0,  0,  0,  0,  0,  0,  0,
		    5, 10, 10,-20,-20, 10, 10,  5,
		    5, -5,-10,  0,  0,-10, -5,  5,
		    0,  0,  0, 20, 20,  0,  0,  0,
		    5,  5, 10, 25, 25, 10,  5,  5,
			10, 10, 20, 30, 30, 20, 10, 10,
			50, 50, 50, 50, 50, 50, 50, 50,
			0,  0,  0,  0,  0,  0,  0,  0
	   }
	};

	extern const int knightTable[2][64] = {
	   {
		   -50,-40,-30,-30,-30,-30,-40,-50,
		   -40,-20,  0,  0,  0,  0,-20,-40,
		   -30,  0, 10, 15, 15, 10,  0,-30,
		   -30,  5, 15, 20, 20, 15,  5,-30,
		   -30,  0, 15, 20, 20, 15,  0,-30,
		   -30,  5, 10, 15, 15, 10,  5,-30,
		   -40,-20,  0,  5,  5,  0,-20,-40,
		   -50,-25,-30,-30,-30,-30,-25,-50
	   },
	   {
		   -50,-25,-30,-30,-30,-30,-25,-50,
		   -40,-20,  0,  5,  5,  0,-20,-40,
		   -30,  5, 10, 15, 15, 10,  5,-30,
		   -30,  0, 15, 20, 20, 15,  0,-30,
		   -30,  5, 15, 20, 20, 15,  5,-30,
		   -30,  0, 10, 15, 15, 10,  0,-30,
		   -40,-20,  0,  0,  0,  0,-20,-40,
		   -50,-40,-30,-30,-30,-30,-40,-50
	   }
	};

	extern const int bishopTable[2][64] = {
	   {
		   -20,-10,-10,-10,-10,-10,-10,-20,
		   -10,  0,  0,  0,  0,  0,  0,-10,
		   -10,  0,  5, 10, 10,  5,  0,-10,
		   -10,  5,  5, 10, 10,  5,  5,-10,
		   -10,  0, 10, 10, 10, 10,  0,-10,
		   -10, 10, 10, 10, 10, 10, 10,-10,
		   -10,  5,  0,  0,  0,  0,  5,-10,
		   -20,-10,-10,-10,-10,-10,-10,-20
	   },
	   {
		   -20,-10,-10,-10,-10,-10,-10,-20,
		   -10,  5,  0,  0,  0,  0,  5,-10,
		   -10, 10, 10, 10, 10, 10, 10,-10,
		   -10,  0, 10, 10, 10, 10,  0,-10,
		   -10,  5,  5, 10, 10,  5,  5,-10,
		   -10,  0,  5, 10, 10,  5,  0,-10,
		   -10,  0,  0,  0,  0,  0,  0,-10,
		   -20,-10,-10,-10,-10,-10,-10,-20
	   }
	};

	extern const int rookTable[2][64] = {
	   {
		   0,  0,  0,  0,  0,  0,  0,  0,
			5, 10, 10, 10, 10, 10, 10,  5,
		   -5,  0,  0,  0,  0,  0,  0, -5,
		   -5,  0,  0,  0,  0,  0,  0, -5,
		   -5,  0,  0,  0,  0,  0,  0, -5,
		   -5,  0,  0,  0,  0,  0,  0, -5,
		   -5,  0,  0,  0,  0,  0,  0, -5,
			0,  0,  0,  5,  5,  0,  0,  0
	   },
	   {
		   0,  0,  0,  5,  5,  0,  0,  0,
		   -5,  0,  0,  0,  0,  0,  0, -5,
		   -5,  0,  0,  0,  0,  0,  0, -5,
		   -5,  0,  0,  0,  0,  0,  0, -5,
		   -5,  0,  0,  0,  0,  0,  0, -5,
		   -5,  0,  0,  0,  0,  0,  0, -5,
		   5, 10, 10, 10, 10, 10, 10,  5,
		   0,  0,  0,  0,  0,  0,  0,  0
	   }
	};

	extern const int kingTable[2][64] = {
	   {
		   -30,-40,-40,-50,-50,-40,-40,-30,
		   -30,-40,-40,-50,-50,-40,-40,-30,
		   -30,-40,-40,-50,-50,-40,-40,-30,
		   -30,-40,-40,-50,-50,-40,-40,-30,
		   -20,-30,-30,-40,-40,-30,-30,-20,
		   -10,-20,-20,-20,-20,-20,-20,-10,
			20, 20,  0,  0,  0,  0, 20, 20,
			20, 30, 10,  0,  0, 10, 30, 20
	   },
	   {
		   20, 30, 10,  0,  0, 10, 30, 20,
		   20, 20,  0,  0,  0,  0, 20, 20,
		   -10,-20,-20,-20,-20,-20,-20,-10,
		   -20,-30,-30,-40,-40,-30,-30,-20,
		   -30,-40,-40,-50,-50,-40,-40,-30,
		   -30,-40,-40,-50,-50,-40,-40,-30,
		   -30,-40,-40,-50,-50,-40,-40,-30,
		   -30,-40,-40,-50,-50,-40,-40,-30
	   }
	};

	extern const int kingEndgameTable[2][64] = {
	   {
		   -50,-40,-30,-20,-20,-30,-40,-50,
		   -30,-20,-10,  0,  0,-10,-20,-30,
		   -30,-10, 20, 30, 30, 20,-10,-30,
		   -30,-10, 30, 40, 40, 30,-10,-30,
		   -30,-10, 30, 40, 40, 30,-10,-30,
		   -30,-10, 20, 30, 30, 20,-10,-30,
		   -30,-30,  0,  0,  0,  0,-30,-30,
		   -50,-30,-30,-30,-30,-30,-30,-50
	   },
	   {
		   -50,-30,-30,-30,-30,-30,-30,-50,
		   -30,-30,  0,  0,  0,  0,-30,-30,
		   -30,-10, 20, 30, 30, 20,-10,-30,
		   -30,-10, 30, 40, 40, 30,-10,-30,
		   -30,-10, 30, 40, 40, 30,-10,-30,
		   -30,-10, 20, 30, 30, 20,-10,-30,
		   -30,-20,-10,  0,  0,-10,-20,-30,
		   -50,-40,-30,-20,-20,-30,-40,-50
	   }
	};

	extern const int queenTable[2][64] = {
	   {
		   -20,-10,-10, -5, -5,-10,-10,-20,
		   -10,  0,  0,  0,  0,  0,  0,-10,
		   -10,  0,  5,  5,  5,  5,  0,-10,
			-5,  0,  5,  5,  5,  5,  0, -5,
			 0,  0,  5,  5,  5,  5,  0, -5,
		   -10,  5,  5,  5,  5,  5,  0,-10,
		   -10,  0,  5,  0,  0,  0,  0,-10,
		   -20,-10,-10, -5, -5,-10,-10,-20
	   },
	   {
		   -20,-10,-10, -5, -5,-10,-10,-20,
		   -10,  0,  5,  0,  0,  0,  0,-10,
		   -10,  5,  5,  5,  5,  5,  0,-10,
		   0,  0,  5,  5,  5,  5,  0, -5,
		   -5,  0,  5,  5,  5,  5,  0, -5,
		   -10,  0,  5,  5,  5,  5,  0,-10,
		   -10,  0,  0,  0,  0,  0,  0,-10,
		   -20,-10,-10, -5, -5,-10,-10,-20
	   }
	};

	const int WORST_VALUE = INT32_MIN + 100;
	const int BEST_VALUE = INT32_MAX - 100;

	/*
	 * Checks if the position is special in a way that it does not need to be evaluated. For example
	 * mates and insufficient material.
	 */
	special_board isSpecialBoard(unsigned int side, const chessboard& board, bool legalMoves, int depth) {
		if(board.isInsufficientMaterial()) { //draw without evaluation
			return {true, 0};
		}
		if(!legalMoves) {
			bool inCheck = isAttacked(board, 1-side, board.getKingSquare(side));
			if(inCheck) {
				return {true, WORST_VALUE - depth};
			} else {
				return {true, 0};
			}
		}
		return {false, 0};
	}

	static unsigned int pieceMaterial[7] = {0, 1, 3, 3, 5, 0, 9};

	bool isEndgame(const chessboard& board) {
		unsigned int materialWhite = 0, materialBlack = 0;
		for(unsigned int square = 0; square < 64; ++square) {
			int piece = board[square];
			if(piece != 0) {
				int pieceType = piece > 0 ? piece : -piece;
				if(piece > 0) { //white piece
					materialWhite += pieceMaterial[pieceType];
				} else { //black piece
					materialBlack += pieceMaterial[pieceType];
				}
			}
		}
		return materialWhite <= 13 && materialBlack <= 13;
	}

	/*
	 * Will count from how many diagonals a bishop is locked in BY PAWNS. The less the better
	 */
	int bishopLockedFactor(unsigned int square, const chessboard& board) {
		int pawnsAround = 0;
		for(unsigned int i=0; i<offsetAmount[bishop]; ++i) {
			int direction = offsets[bishop][i];
			for (int n = square;;) {
				n = mailbox[mailbox64[n] + direction]; //next square in this direction
				if(board[n] == 1 || board[n] == -1) { //a pawn found
					++pawnsAround;
				}
				break;
			}
		}
		return pawnsAround;
	}

	/*
	 * Evaluates if a rook is on an open file. It will check what kind of pawns are
	 * on said file.
	 * Returns how good the situation is for "side".
	 */
	int rookFileEvaluation(unsigned int side, unsigned int square, const chessboard& board) {
		bool foundOwnPawn = false, foundEnemyPawn = false;
		int ownPawn = side==white ? 1 : -1;
		int enemyPawn = side==white ? -1 : 1;
		int file = square % 8;
		for(int i = 0; i < 8; ++i) { //iterate file
			unsigned int squareInFile = 8 * i + file;
			int piece = board[squareInFile];
			if(piece == ownPawn) {
				foundOwnPawn = true;
			} else if(piece == enemyPawn) {
				foundEnemyPawn = true;
			}
		}
		if(!foundOwnPawn && !foundEnemyPawn) { //rook on an open file
			return 10;
		} else if(!foundOwnPawn && foundEnemyPawn) {
			return 3;
		} else {
			return 0;
		}
	}

	/*
	 * Checks if a pawn is passed, meaning there are no enemy pawns ahead of it in the same file
	 * or the files right next to it.
	 */
	bool isPassedPawn(unsigned int side, unsigned int square, const chessboard& board) {
		int file = square % 8;
		int enemyPawn = side==white ? -1 : 1;
		int dirForward = side == white ? -10 : 10;
		for (int n = square;;) {
			n = mailbox[mailbox64[n] + dirForward];
			if(n == -1) break;
			if(board[n] == enemyPawn) {
				return false; //enemy pawn in front, can not be passed
			}
		}
		if(file != 0) { //pawn not on leftmost file, can check file to the left for enemy pawns
			for (int n = square-1;;) {
				n = mailbox[mailbox64[n] + dirForward];
				if(n == -1) break;
				if(board[n] == enemyPawn) {
					return false; //enemy pawn in front, can not be passed
				}
			}
		}
		if(file != 7) { //pawn not on rightmost file, can check file to the right for enemy pawns
			for (int n = square+1;;) {
				n = mailbox[mailbox64[n] + dirForward];
				if(n == -1) break;
				if(board[n] == enemyPawn) {
					return false; //enemy pawn in front, can not be passed
				}
			}
		}
		return true; //no enemy pawns found in front
	}

	/*
	 * Checks if there are doubled pawns on the file of 'square'. Returns the number of
	 * doubled pawns from the given side on the file. If there are no doubled pawns it will
	 * return 0.
	 */
	unsigned int doubledPawnEvaluation(unsigned int side, unsigned int square, const chessboard& board) {
		unsigned int dPawnCount = 0;
		int ownPawn = side==white ? 1 : -1;
		int file = square % 8;
		for(int i = 0; i < 8; ++i) { //iterate file
			unsigned int squareInFile = 8 * i + file;
			int piece = board[squareInFile];
			if(piece == ownPawn) { //another one of our pawns found on this file
				++dPawnCount;
			}
		}
		return dPawnCount < 2 ? 0 : dPawnCount;
	}

	/*
	 * Checks if the pawn is isolated, meaning it has no friendly pawns on the neighbouring squares.
	 */
	bool isIsolatedPawn(unsigned int side, unsigned int square, const chessboard& board) {
		int ownPawn = side==white ? 1 : -1;
		for(unsigned int i = 0; i < offsetAmount[queen]; ++i) { //can get all neightboard using the queens directions
			for (int n = square;;) {
				n = mailbox[mailbox64[n] + offsets[queen][i]];
				if(n == -1) break;
				if(board[n] == ownPawn) {
					return false; //own pawn found in the neighbordhood, cant be isolated
				}
				break; //only go to distance 1
			}
		}
		return true;
	}

	/*
	 * Evaluates king safety by counting friendly and enemy pieces in the kings quadrant of the board.
	 * Queen counts as 3 pieces.
	 * The higher the return value, the safer it is for the side.
	 */
	int kingSafetyEvaluation(unsigned int side, unsigned int square, const chessboard& board) {
		int file = square % 8;
		int rank = square / 8;
		/*
		 * determine quadrant of the king
		 *  ----------------------
		 *  |         |          |
		 *  |  q1     |    q2    |
		 *  |---------|----------|
		 *  |  q3     |    q4    |
		 *  |         |          |
		 *  ----------------------
		 */
		unsigned int fromFile, toFile, fromRank, toRank;
		if(file < 4) {
			fromFile = 0;
			toFile = 3;
			if(rank < 4) { //q1
				fromRank = 0;
				toRank = 3;
			} else { //q3
				fromRank = 4;
				toRank = 7;
			}
		} else {
			fromFile = 4;
			toFile = 7;
			if(rank < 4) { //q2
				fromRank = 0;
				toRank = 3;
			} else { //q4
				fromRank = 4;
				toRank = 7;
			}
		}
		int friendlyPieces = 0, enemyPieces = 0;
		//count pieces in the quadrant
		for(unsigned int i=fromRank; i<=toRank; ++i) {
			for(unsigned int j=fromFile; j<=toFile; ++j) {
				unsigned int checkedSquare = (i * 8) + j;
				if(checkedSquare == square) continue; //king does not count
				int piece = board[checkedSquare]; //piece (or empty) at this position
				if(piece != 0) {
					unsigned int sideOfPiece = piece > 0 ? white : black;
					if(piece == 6 || piece == -6) { //this is a queen, counts as 3
						sideOfPiece == side ? friendlyPieces+=3 : enemyPieces+=3;
					} else { //not a queen, counts as 1
						sideOfPiece == side ? ++friendlyPieces : ++enemyPieces;
					}
				}

			}
		}
		return 5 * (friendlyPieces - enemyPieces);
	}

	int evaluateBoard(unsigned int side, const chessboard& board, const game_information& info, unsigned int movesAmount) {
		//int sideMultiplier = side == white ? 1 : -1;
		unsigned int enemySide = 1-side;

		int pieceCounts[2][7] = { //stores how many of each piece we found
				{0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0, 0, 0}
		};

		//Used to only count stacking "doubled" pawns on a file once.
		bool doubledPawnCheckedFiles[8] = {false, false, false, false,false, false, false, false};

		/*
		 * No mates, sufficient material, can begin material/mobility evaluation.
		 * The higher the score, the better this position is for the side to move.
		 */
		bool endgame = isEndgame(board);
		//detect if we are in check
		bool inCheck = isAttacked(board, enemySide, board.getKingSquare(side));
		//detect if enemy is inCheck
		bool enemyInCheck = isAttacked(board, side, board.getKingSquare(enemySide));
		//begin evaluation with material and positional evaluation
		int evaluation = 0;
		for(unsigned int square = 0; square < 64; ++square) {
			int piece = board[square];
			if(piece != 0) { //piece here
				unsigned int pieceType = piece > 0 ? piece : -piece;
				unsigned int sideOfPiece = piece > 0 ? white : black;
				// Material evaluation
				if(sideOfPiece == side) { //a friendly piece
					evaluation += pieceValues[pieceType];
				} else { //an enemy piece
					evaluation -= pieceValues[pieceType];
				}
				// piece square table evaluation + other piece specific
				if(pieceType == pawn) {
					evaluation += pawnTable[sideOfPiece][square];
					if(isPassedPawn(sideOfPiece, square, board)) { //this pawn is passed
						if(sideOfPiece == side) { //our passed pawn
							evaluation += endgame ? 20 : 10;
						} else { //enemy passed pawn
							evaluation -= endgame ? 20 : 10;
						}
					}
					//check for doubled pawns, if not found doubled pawns on this file already
					if(!doubledPawnCheckedFiles[square%8]) {
						unsigned int dp = doubledPawnEvaluation(sideOfPiece, square, board);
						if(sideOfPiece == side) { //our doubled pawns
							evaluation -= dp * 8;
						} else { //enemy doubled pawns
							evaluation += dp * 8;
						}
						doubledPawnCheckedFiles[square%8] = true; //so we wont check this file again
					}
					if(isIsolatedPawn(sideOfPiece, square, board)) { //check for isolation
						if(sideOfPiece == side) { //our isolated pawn
							evaluation -= 2;
						} else { //enemy isolated pawn
							evaluation += 2;
						}
					}

				} else if(pieceType == knight) {
					evaluation += knightTable[sideOfPiece][square];
				} else if(pieceType == bishop) {
					evaluation += bishopTable[sideOfPiece][square];
					int pawnsAround = bishopLockedFactor(square, board);
					if(sideOfPiece == side) { //our bishop
						evaluation -= (10*pawnsAround); //not good if pawns are around bishop
					} else { //enemy bishop
						evaluation += (10*pawnsAround); //good if pawns are around enemy bishop
					}
				} else if(pieceType == rook) {
					evaluation += rookTable[sideOfPiece][square];
					int rfe = rookFileEvaluation(sideOfPiece, square, board);
					if(sideOfPiece == side) { //evaluate rook file openness
						evaluation += rfe;
					} else {
						evaluation -= rfe;
					}
				} else if(pieceType == king) {
					if(endgame) {
						evaluation += kingEndgameTable[sideOfPiece][square];
					} else {
						evaluation += kingTable[sideOfPiece][square];
					}
					int kingSafety = kingSafetyEvaluation(sideOfPiece, square, board);
					if(sideOfPiece == side) { //our king
						 evaluation += inCheck ? kingSafety - 20 : kingSafety;
					} else { //enemy king
						evaluation -= enemyInCheck ? kingSafety + 20 : kingSafety;
					}
				} else { //must be queen
					evaluation += queenTable[sideOfPiece][square];
				}
			}
		}
		if(pieceCounts[side][bishop] >= 2) { //reward for bishop pair
			evaluation += 15;
		}
		if(pieceCounts[enemySide][bishop] >= 2) { //penalty for enemy bishop pair
			evaluation -= 15;
		}
		//penalize side that has no castled in the early game
		if(!endgame && !info.getHasCastled(side)) {
			evaluation -= (info.getKingsideCastleRights(side)||info.getQueensideCastleRights(side)) ? 15 : 25;
		}
		if(!endgame && !info.getHasCastled(enemySide)) {
			evaluation += (info.getKingsideCastleRights(enemySide)||info.getQueensideCastleRights(enemySide)) ? 15 : 25;
		}
		//Mobility: warning: one sided
		if(!inCheck) { //if we are not in check then almost all pseudo legal moves will be legal
			evaluation += 2 * movesAmount;
		}
		return evaluation;
	}
}

