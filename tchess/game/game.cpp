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

	game::game(char whiteCode, char blackCode, TChessRootDialogView* view, bool wait) : gameEnded(false), waitWithMoves(wait), 
		awaitingCalculation(false), awaitingGui(false), illegalMoveCounter{ ALLOWED_ILLEGAL_MOVES, ALLOWED_ILLEGAL_MOVES }, view(view)
	{
		switch (whiteCode)
		{
		case humanPlayerCode:
			whitePlayer = new human_player_gui(white, view);
			break;
		case randomPlayerCode:
			whitePlayer = new random_player(white, view);
			break;
		case greedyPlayerCode:
			whitePlayer = new greedy_player(white, view);
			break;
		case engineCode:
			whitePlayer = new engine(white, view);
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
			blackPlayer = new random_player(black, view);
			break;
		case greedyPlayerCode:
			blackPlayer = new greedy_player(black, view);
			break;
		case engineCode:
			blackPlayer = new engine(black, view);
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

	void game::startGame()
	{
		//Redraw the board, in case there is previous game drawn
		drawBoard(board, view->squareControls);
		//update the UI
		CString newStatus(_T("Game is ongoing"));
		view->gameStatus.SetWindowText(newStatus);
		CString wn = whitePlayer->description().c_str();
		view->whitePlayerName.SetWindowText(wn);
		CString bn = blackPlayer->description().c_str();
		view->blackPlayerName.SetWindowText(bn);
		CString turnToMove(_T("WHITE's turn to move"));
		view->turnToMove.SetWindowText(turnToMove);
		if (whitePlayer->isGuiInteractive() || blackPlayer->isGuiInteractive()) {
			view->resignButton.EnableWindow(true); //enable resign button if one player can use it.
		}
		//disable engine difficulty setting midgame
		view->radioDiff2.EnableWindow(false);
		view->radioDiff3.EnableWindow(false);
		view->radioDiff4.EnableWindow(false);
		view->radioDiff5.EnableWindow(false);
		view->radioDiff6.EnableWindow(false);

		//if the make move button is not used or the next player uses the GUI then move can be instantly made
		bool nextPlayerGuiInteractive = info.getSideToMove() == white ? whitePlayer->isGuiInteractive() : blackPlayer->isGuiInteractive();
		if (!waitWithMoves || nextPlayerGuiInteractive) {
			nextMove();
		}
	}

	//This struct is passed in to the background thread
	struct thread_param {
		player* playerWhoMoves;
		game* gameController;
		thread_param(player* p, game* g) : playerWhoMoves(p), gameController(g) {}
	};

	/*
	* The background process that calculates a non GUI interactive players move. The parameter is a pointer 
	* to a thread_param object.
	*/
	UINT calculateMove(LPVOID param) {
		thread_param* tp = (thread_param*)param;
		move m = tp->playerWhoMoves->makeMove(tp->gameController);
		//allocate a move object
		move* movePointer = new move(m);
		//post message with the calculated move
		PostMessage(tp->gameController->getView()->GetSafeHwnd(), MOVE_CALCULATED_MESSAGE, 0, reinterpret_cast<LPARAM>(movePointer));
		//free memory, return sucess
		delete param;
		return 0;
	}

	void game::nextMove()
	{
		if (info.getSideToMove() == white) {
			//white to move
			if (!whitePlayer->isGuiInteractive()) {
				//launch thread that calculates the move
				thread_param* param = new thread_param(whitePlayer, this);
				AfxBeginThread(calculateMove, param);
				view->moveGenerationStatusText.SetWindowText(_T("Move generation ongoing..."));
				view->moveGenerationProgress.SetPos(0);
				awaitingCalculation = true; //the controller is now waiting for the move from the GUI or from the background thread
			}
			else {
				//player uses gui, the 'submitMove' method will be called
				awaitingGui = true;
			}
			
		}
		else {
			//black to move
			if (!blackPlayer->isGuiInteractive()) {
				//launch thread that calculates the move
				thread_param* param = new thread_param(blackPlayer, this);
				AfxBeginThread(calculateMove, param);
				view->moveGenerationStatusText.SetWindowText(_T("Move generation ongoing..."));
				view->moveGenerationProgress.SetPos(0);
				awaitingCalculation = true; //the controller is now waiting for the move from the GUI or from the background thread
			}
			else {
				awaitingGui = true;
			}
		}
		
	}

	void game::submitMove(const move& m)
	{
		awaitingCalculation = false; //move received
		awaitingGui = false;
		acceptMove(m);
		drawBoard(board, view->squareControls);
		if (!gameEnded) {
			bool nextPlayerGuiInteractive = info.getSideToMove() == white ? whitePlayer->isGuiInteractive() : blackPlayer->isGuiInteractive();
			if (!nextPlayerGuiInteractive && waitWithMoves) {
				view->makeMoveButton.EnableWindow(true);
			}
			//if the make move button is not used or the next player uses the GUI then move can be instantly made
			if (!waitWithMoves || nextPlayerGuiInteractive) {
				nextMove();
			}
		}
	}

	//Specifically for the human player, who does not have to enter if a move is capture or not
	void captureFix(unsigned int side, const chessboard& board, move& m) {
		int atDest = board[m.getToSquare()];
		int piece = board[m.getFromSquare()];
		int fileFrom = m.getFromSquare() % 8;
		int fileTo = m.getToSquare() % 8;
		if((side==white && atDest < 0) || (side==black && atDest > 0)) {
			//appears to be capture
			if(!m.isPromotion()) {
				//not a promotion
				m = move(m.getFromSquare(), m.getToSquare(), capture, 0);
			} else {
				//promotion capture, replace promotion with promotion capture
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
		else if ((piece == -1 || piece == 1) && atDest == 0 && fileFrom != fileTo) {
			//appears to be en passant: pawn move to empty square CHANGING files
			m = move(m.getFromSquare(), m.getToSquare(), enPassantCapture, 0);
		}
	}

	void game::acceptMove(move m)
	{
		unsigned int side = info.getSideToMove();
		std::string playerWhoMoves = side == white ? whitePlayer->description() : blackPlayer->description();

		if (m.isResign()) { //check if player resigned before going any further
			std::string playerWhoWon = side == white ? blackPlayer->description() : whitePlayer->description();
			endGame(false, side == white ? black : white, "Resignation");
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

		if (result.isLegal()) { //move is legal
			//update game information, such as castling rights and side to move
			updateGameInformation(board, m, info);
			/*
			 * Check if the game has ended: checkmate, stalemate, repetition, etc.
			 * For this, all legal moves of the side to move is needed.
			 * Also need to know is the side to move is in check.
			 */
			bool check = isAttacked(board, side, board.getKingSquare(1 - side)), checkmate = false, stalemate = false;
			generator.generatePseudoLegalMoves(1 - side, pseudoLegalMoves); //all pseudo legal moves of the side to move
			unsigned int legalMoveCount = 0;
			for (const move& plMove : pseudoLegalMoves) {
				move_legality_result legRes = isValidMove(plMove, pseudoLegalMoves); //this made the move on the board!!!
				if (legRes.isLegal()) { //found at least 1 legal move, cant be stalemate, checkmate
					++legalMoveCount;
					board.unmakeMove(plMove, 1 - side, legRes.getCapturedPiece()); //unmake tested move
					break;
				}
				board.unmakeMove(plMove, 1 - side, legRes.getCapturedPiece()); //unmake tested move
			}
			if (legalMoveCount == 0) { //no legal moves, must be checkmate or stalemate
				if (check) {
					checkmate = true;
				}
				else {
					stalemate = true;
				}
			}
			//TODO write moves to UI
			std::string appendToMove;
			if (checkmate) {
				appendToMove += "#";
				displayMoveOnGui(pieceThatMoved, appendToMove);
				endGame(false, side, "Checkmate");
				return;
			}
			else if (stalemate) {
				displayMoveOnGui(pieceThatMoved, appendToMove);
				endGame(true, 0, "Stalemate");
				return;
			}
			else if (board.isInsufficientMaterial()) {
				displayMoveOnGui(pieceThatMoved, appendToMove);
				endGame(true, 0, "Insufficient mating material");
				return;
			}
			else if (check) {
				appendToMove += "+";
			}
			moves.push_back(m); //save this move
			//save extra info about move
			move_extra_info extra;
			extra.appendToMove = appendToMove;
			extra.pieceThatMoved = pieceThatMoved;
			moveExtras.push_back(extra);
			//display move on the GUI
			displayMoveOnGui(pieceThatMoved, appendToMove);
			//it is the next players turn to move
			CString turnToMove(side == white ? _T("BLACK's turn to move") : _T("WHITE's turn to move"));
			view->turnToMove.SetWindowText(turnToMove);
		}
		else { //move is illegal
			if (result.isPseudoLegal()) {  //unmake the illegal move on the board
				board.unmakeMove(m, side, result.getCapturedPiece());
			}
			--illegalMoveCounter[side];
			CString illMoveMessage;
			illMoveMessage.Format(_T("%s has made an illegal move: %s. Reason: %s"), CString(playerWhoMoves.c_str()).GetBuffer(),
				CString(m.to_string(pieceThatMoved).c_str()).GetBuffer(), CString(result.getInformation().c_str()).GetBuffer());
			AfxMessageBox(illMoveMessage, MB_OK | MB_ICONERROR);
			if (illegalMoveCounter[side] > 0) {
				CString message;
				message.Format(_T("%s can only make %u more illegal moves before losing!"), CString(playerWhoMoves.c_str()).GetBuffer(), illegalMoveCounter[side]);
				AfxMessageBox(message, MB_OK | MB_ICONWARNING);
			}
			else { //this player made too many illegal moves and loses
				CString message;
				message.Format(_T("%s has made too many illegal moves, and lost!"), CString(playerWhoMoves.c_str()).GetBuffer());
				AfxMessageBox(message, MB_OK | MB_ICONWARNING);
				endGame(false, side == white ? black : white, "Too many illegal moves!");
			}
		}
	}

	const std::vector<move>& game::getMoves() const {
		return moves;
	}

	bool game::gameOngoing() const
	{
		return !gameEnded;
	}

	move_legality_result game::isValidMove(const move& playerMove, std::vector<move>& pseudoLegalMoves) {
		bool legal = false;
		bool pseudoLegal = false;
		std::string information = "This move is not legal for this side!";
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

	void game::endGame(bool draw, unsigned int winningSide, const std::string& message) {
		gameEnded = true;
		view->gameStatus.SetWindowText(_T("Game has ended")); //update status
		if(draw) { //update result
			view->gameResult.SetWindowText(_T("1/2 - 1/2"));
		} else {
			view->gameResult.SetWindowText(winningSide == white ? _T("1 - 0") : _T("0 - 1"));
		}
		//update reason
		CString cMessage = message.c_str();
		view->gameResultReason.SetWindowText(cMessage);
		//hide turn to move
		CString empty(_T(""));
		view->turnToMove.SetWindowText(empty);
		//disable buttons that  should only be used during a game
		view->makeMoveButton.EnableWindow(false);
		view->resignButton.EnableWindow(false);
		//enable difficulty change
		view->radioDiff2.EnableWindow(true);
		view->radioDiff3.EnableWindow(true);
		view->radioDiff4.EnableWindow(true);
		view->radioDiff5.EnableWindow(true);
		view->radioDiff6.EnableWindow(true);

		//After game ended and UI is updated, make a save game object
		view->createSaveGame();
	}

	void game::displayMoveOnGui(unsigned int pieceThatMoved, const std::string& appendThis)
	{
		//determine who made the last move
		UINT sideThatMoved = 1 - info.getSideToMove();
		UINT lastRowIndex = view->moveList.GetItemCount() - 1;
		if (lastRowIndex < 0) lastRowIndex = 0;
		move& lastMove = moves[moves.size() - 1];
		CString moveString;
		moveString.Format(_T("%s %s"), CString(lastMove.to_string(pieceThatMoved).c_str()).GetBuffer(), CString(appendThis.c_str()).GetBuffer());

		if (sideThatMoved == white) {
			//white moved, need to insert a new row
			view->moveList.InsertItem(LVCF_TEXT, lastRowIndex+1, _T(""), 0, 0, 0 ,0);
			CString  numText;
			numText.Format(_T("%d"), (moves.size() / 2) + 1);
			//modify the num column with the number
			view->moveList.SetItemText(lastRowIndex+1, 0, numText);
			//modify the white column with move string
			view->moveList.SetItemText(lastRowIndex+1, 1, moveString);
		}
		else {
			//black moved, need to fill "black" column of the last row
			view->moveList.SetItemText(lastRowIndex, 2, moveString);
		}
	}
}




