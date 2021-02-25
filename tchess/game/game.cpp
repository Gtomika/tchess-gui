/*
 * game.cpp
 *
 *  Created on: 2021. jan. 30.
 *      Author: Gáspár Tamás
 */

#include "game.h"

#include "human/human_player_gui.h"
#include "engine/engine.h"
#include "random/random_player.h"
#include "greedy/greedy_player.h"

namespace tchess
{
	const char humanPlayerCode = 'P';
	const char randomPlayerCode = 'R';
	const char greedyPlayerCode = 'G';
	const char engineCode = 'E';

	//game class implementation

	game::game(char whiteCode, char blackCode, const TChessRootDialogView* view) : gameEnded(false), startNewGame(false), 
		illegalMoveCounter{ ALLOWED_ILLEGAL_MOVES, ALLOWED_ILLEGAL_MOVES }, view(view)
	{
		switch (whiteCode)
		{
		case humanPlayerCode:
			whitePlayer = new human_player_gui(white, view);
			break;
		case randomPlayerCode:
			whitePlayer = new random_player(white);
			break;
		case greedyPlayerCode:
			whitePlayer = new greedy_player(white);
			break;
		case engineCode:
			whitePlayer = new engine(white);
			break;
		default:
			throw std::runtime_error("Invalid player code: " + whiteCode);
			whitePlayer = nullptr;
			break;
		}
		switch (blackCode)
		{
		case humanPlayerCode:
			blackPlayer = new human_player_gui(black, view);
			break;
		case randomPlayerCode:
			blackPlayer = new random_player(black);
			break;
		case greedyPlayerCode:
			blackPlayer = new greedy_player(black);
			break;
		case engineCode:
			blackPlayer = new engine(black);
			break;
		default:
			throw std::runtime_error("Invalid player code: " + blackCode);
			blackPlayer = nullptr;
			break;
		}
	}

	game::~game() 
	{
		delete whitePlayer;
		delete blackPlayer;
	}

	bool game::playGame() {
		std::cout << "The game begins: " << whitePlayer->description() << " vs " << blackPlayer->description() << std::endl;
		std::cout << board.to_string();
		while(!gameEnded) { //the endGame method will set this to true
			acceptMove();
		}
		return startNewGame;
	}

	//Specifically for the human player, who does not have to enter if a move is capture or not
	void captureFix(unsigned int side, const chessboard& board, move& m) {
		if(m.isEnPassant()) return;
		int atDest = board[m.getToSquare()];
		if((side==white && atDest < 0) || (side==black && atDest > 0)) {
			//appears to be capture
			if(!m.isPromotion()) {
				m = move(m.getFromSquare(), m.getToSquare(), capture, 0);
			} else {
				//replace promotion with promotion capture
				unsigned int promTo = m.promotedTo();
				unsigned int promCapTo;
				if(promTo == queen) {
					promCapTo = queenPromotionCap;
				} else if(promTo == rook) {
					promCapTo = rookPromotionCap;
				} else if(promTo == bishop) {
					promCapTo = bishopPromotionCap;
				} else {
					promCapTo = knightPromotionCap;
				}
				m = move(m.getFromSquare(), m.getToSquare(), promCapTo, 0);
			}

		}
	}

	void game::acceptMove() {
		unsigned int side = info.getSideToMove();
		std::string playerWhoMoves = side==white ? whitePlayer->description() : blackPlayer->description();
		std::cout << playerWhoMoves << "'s turn to move..." << std::endl;
		//get the next move
		move m = side == white ? whitePlayer->makeMove(this) : blackPlayer->makeMove(this);

		if(m.isResign()) { //check if player resigned before going any further
			std::string playerWhoWon = side==white ? blackPlayer->description() : whitePlayer->description();
			startNewGame = endGame(false, playerWhoWon, "Resignation");
			return;
		}

		//correct this move if it appears to be a capture (only important for human player)
		captureFix(side, board, m);

		std::vector<move> pseudoLegalMoves; //generate pseudo legal moves, will be needed at least for move validation
		move_generator generator(board, info);
		generator.generatePseudoLegalMoves(side, pseudoLegalMoves);

		int pieceThatMoved = board[m.getFromSquare()]; //will be needed later, if the move is legal (only the type)
		pieceThatMoved = pieceThatMoved >= 0 ? pieceThatMoved : -pieceThatMoved;
		move_legality_result result = isValidMove(m, pseudoLegalMoves); //make the move on the board while checking

		if(result.isLegal()) { //move is legal
			moves.push_back(m); //save this move
			//update game information, such as castling rights and side to move
			updateGameInformation(board, m, info);
			/*
			 * Check if the game has ended: checkmate, stalemate, repetition, etc.
			 * For this, all legal moves of the side to move is needed.
			 * Also need to know is the side to move is in check.
			 */
			bool check = isAttacked(board, side, board.getKingSquare(1-side)), checkmate = false, stalemate = false;
			generator.generatePseudoLegalMoves(1-side, pseudoLegalMoves); //all pseudo legal moves of the side to move
			unsigned int legalMoveCount = 0;
			for(const move& plMove: pseudoLegalMoves) {
				move_legality_result legRes = isValidMove(plMove, pseudoLegalMoves); //this made the move on the board!!!
				if(legRes.isLegal()) { //found at least 1 legal move, cant be stalemate, checkmate
					++legalMoveCount;
					board.unmakeMove(plMove, 1-side, legRes.getCapturedPiece()); //unmake tested move
					break;
				}
				board.unmakeMove(plMove, 1-side, legRes.getCapturedPiece()); //unmake tested move
			}
			if(legalMoveCount == 0) { //no legal moves, must be checkmate or stalemate
				if(check) {
					checkmate = true;
				} else {
					stalemate = true;
				}
			}
			std::cout << "Move " << moves.size() << ": " << playerWhoMoves <<
					" has made the move: " << m.to_string(pieceThatMoved);
			if(checkmate) {
				std::cout << " (checkmate)" << std::endl;
				startNewGame = endGame(false, playerWhoMoves, "Checkmate");
				return;
			} else if(stalemate) {
				std::cout << " (stalemate)" << std::endl;
				startNewGame = endGame(true, "", "Stalemate");
				return;
			} else if(board.isInsufficientMaterial()) {
				std::cout << std::endl;
				startNewGame = endGame(true, "", "Insufficient mating material");
				return;
			} else if(check) {
				std::cout << " (check)";
			}
			std::cout << std::endl;
			std::cout << board.to_string();
		} else { //move is illegal
			if(result.isPseudoLegal()) {  //unmake the illegal move on the board
				board.unmakeMove(m, side, result.getCapturedPiece());
			}
			--illegalMoveCounter[side];
			std::cout << playerWhoMoves << " has made an illegal move: " << m.to_string(pieceThatMoved) << ", " <<
					result.getInformation() << std::endl;
			if(illegalMoveCounter[side] > 0) {
				std::cout << playerWhoMoves << " can only make " << illegalMoveCounter[side] << " more illegal moves before losing!" << std::endl;
			} else { //this player made too many illegal moves and loses
				std::string enemyName = side == white ? blackPlayer->description() : whitePlayer->description();
				endGame(false, enemyName, "Too many illegal moves!");
			}
		}
	}

	const std::vector<move>& game::getMoves() const {
		return moves;
	}

	move_legality_result game::isValidMove(const move& playerMove, std::vector<move>& pseudoLegalMoves) {
		bool legal = false;
		bool pseudoLegal = false;
		std::string information;
		int capturedPiece = 0;
		unsigned int side = info.getSideToMove();
		unsigned int enemySide = 1 - side;
		for(const move& plMove: pseudoLegalMoves) { //check if move is at least pseudo legal
			if(playerMove == plMove) {
				pseudoLegal = true;
				//move is pseudo legal
				capturedPiece = board.makeMove(playerMove, side); //make this move on the board
				if(playerMove.isKingsideCastle()) {
					if(info.getKingsideCastleRights(side)) {
						//for castle, not only king safety needs to be checked, but also the king cant move through attacked squares
						if(!isAttacked(board, enemySide, playerMove.getFromSquare()) && // <-- check that king did not castle out of check
						   !isAttacked(board, enemySide, playerMove.getFromSquare()+1) && // <-- check that king did not castle through check
						   !isAttacked(board, enemySide, playerMove.getFromSquare()+2)) { // <-- check that king did not castle into check
							//the king is safe and did not catle out/through attacked squares
							legal = true;
						} else {
							information = "Can't castle out of/through check!";
						}
					} else { //castle rights lost
						information = "Don't have kingside castle rights anymore!";
					}
					break;
				} else if(playerMove.isQueensideCastle()) {
					//for castle, not only king safety needs to be checked, but also the king cant move through attacked squares
					if(info.getQueensideCastleRights(side)) {
						if(!isAttacked(board, enemySide, playerMove.getFromSquare()) && // <-- check that king did not castle out of check
							   !isAttacked(board, enemySide, playerMove.getFromSquare()-1) && // <-- check that king did not castle through check
							   !isAttacked(board, enemySide, playerMove.getFromSquare()-2)) { // <-- check that king did not castle into check
								//the king is safe and did not catle out/through attacked squares
								legal = true;
							} else {
								information = "Can't castle out of/through check!";
							}
					} else { //castle rights lost
						information = "Don't have queenside castle rights anymore!";
					}
					break;
				} else {
					//not a castle, only king safety is important
					if(!isAttacked(board, enemySide, board.getKingSquare(side))) {
						//the king is safe after this move
						legal = true;
					} else {
						information = "This move leave the king in check!";
					}
					break;
				}
			}
		}
		return move_legality_result(legal, pseudoLegal, information, capturedPiece);
	}

	bool game::endGame(bool draw, const std::string& winninSide, const std::string& message) {
		gameEnded = true;
		if(draw) {
			std::cout << "The game has ended in a draw!" << std::endl;
		} else {
			std::cout << winninSide << " has won the game!" << std::endl;
		}
		std::cout << "Reason: " << message << std::endl
				<< "The board at the end of the game:\n" << board.to_string();
		std::cout << "-------------------------------------" << std::endl;
		std::cout << "Do you want to start a new game? (y = yes/anything else = no)" << std::endl;
		std::string input;
		std::getline(std::cin, input);
		return input == "y";
	}
}




