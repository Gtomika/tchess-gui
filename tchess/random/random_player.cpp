/*
 * random_player.cpp
 *
 *  Created on: 2021. jan. 30.
 *      Author: Gáspár Tamás
 */
#include <random>
#include <iterator>
#include <algorithm>

#include "random_player.h"

namespace tchess
{
	template<typename Iter, typename RandomGenerator>
	Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
		std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
		std::advance(start, dis(g));
		return start;
	}

	//random element selector from stack overflow
	template<typename Iter>
		Iter select_randomly(Iter start, Iter end) {
		static std::random_device rd;
		static std::mt19937 gen(rd());
		return select_randomly(start, end, gen);
	}

	move random_player::makeMove(game* gameController) {
		const std::vector<move>& gameMoves = gameController->getMoves();
		if(gameMoves.size() > 0) {
			move enemyMove = gameMoves.back(); //update our board with enemy move
			board.makeMove(enemyMove, 1-side);
			updateGameInformation(board, enemyMove, info); //update game information
		}
		std::vector<move> moves;
		move_generator generator(board, info);
		generator.generatePseudoLegalMoves(side, moves); //generate all pseudo legal moves
		//filter out illegal moves
		auto legalCheck = [&](const move& m) { return !(isLegalMove(m, board, info)); };
		auto legalEnd = std::remove_if(moves.begin(), moves.end(), legalCheck);

		move randomMove = *select_randomly(moves.begin(), legalEnd); //select legal move randomly
		board.makeMove(randomMove, side); //make own move on own board
		updateGameInformation(board, randomMove, info);
		info.setSideToMove(1-side); //the enemy side is to move now
		return randomMove;
	}

	std::string random_player::description() const {
		return "Random move maker";
	}
}


