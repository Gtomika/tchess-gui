#include "human_player.h"

/*
 * human_player.cpp
 *
 * Implementations for human_player.h
 *
 *  Created on: 2021. jan. 24.
 *      Author: Gáspár Tamás
 */

#include "board/move.h"

namespace tchess
{
	//This message is printed when the user asks for help.
	extern const std::string helpMessage = "Write your move using the piece code and the departure and destination squares!\n"
			" For example, if you want to move your pawn from e2 to e4, then write 'P e2 e4'.\n"
			"Use these piece codes: Q (queen), K (king), N (knight), B (bishop), R (rook), P (pawn)."
			"For special moves, see this list:\n"
			" - Kingside castling: o-o\n"
			" - Queenside castling: o-o-o\n"
			" - Promotions: if your move will result in a promotion, also write what piece you want to promote to!\n"
			" for example, 'P e7 e8 Q' will promote the pawn to a queen.\n"
			" - En passant: you must write 'EP' after your move, for example\n"
			" 'P e5 d6 EP'.\n"
			" - To resign the game, type 'resign'.\n";

	std::string human_player_console::description() const {
		std::string sideName = side == white ? "White" : "Black";
		return "Human controlled player (" + sideName + ")";
	}

	//this player agent does not use the game controller parameter
	move human_player_console::makeMove(const game& gameController) {
		bool moveParsed = false;
		while(!moveParsed) { //if use types help or something unparsable this will be printed again
			std::cout << "--------------------------------" << std::endl;
			std::cout << "Enter your move, or type 'help'!" << std::endl;
			std::string input;
			std::getline(std::cin, input);

			if(input == "help") { //sked for help
				std::cout << helpMessage;
			} else if(input == "resign") {
				moveParsed = true;
				return move(0, 0, resignMove, 0);
			} else { //try to parse a proper move
				try {
					move m = parse_move(input, side);
					moveParsed = true;
					return m; //send move back to game controller
				} catch(move_parse_exception& exc) {
					std::cout << "Could not parse this move: " << exc.what() << std::endl;
				}
			}
		}
		return move(); //will not get here
	}
}




