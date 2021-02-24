/*
 * engine.h
 *
 *	Tchess engine header file.
 *
 *  Created on: 2021. febr. 3.
 *      Author: Gáspár Tamás
 */

#ifndef SRC_ENGINE_ENGINE_H_
#define SRC_ENGINE_ENGINE_H_

#define USE_OPENING_BOOK true

#include "game/player.h"
#include "polyglot.h"
#include "transposition_table.h"
#include "move_ordering.h"

namespace tchess
{
	/*
	 * Default tree search depth.
	 */
	extern const unsigned int default_depth;

	/*
	 * Player agent for the Tchess engine.
	 */
	class engine: public player {

		//Search depth.
		unsigned int depth;

		//Board object.
		chessboard board;

		//game info object.
		game_information info;

		//Stores if the engine is playing opening mode (from book) or not.
		bool opening;

		//Polyglot opening book.
		opening_book openingBook;

		/*
		 * The transposition table used by the engine.
		 */
		transposition_table* ttable;

	public:
		engine() = delete;

		engine(unsigned int side, unsigned int depth = default_depth)
			: player(side, false), depth(depth), opening(USE_OPENING_BOOK) {
			ttable = new transposition_table(def_transposition_table_size);
		}

		~engine() {
			delete ttable;
		}

		/*
		 * Will submit the best move it can find after evaluating the
		 * game tree down to a certain depth.
		 */
		move makeMove(const game& gameController);

		std::string description() const;

	private:
		/*
		 * The root negamax function. Unlike the normal negamax, this
		 * will return the best move instead of the best evaluation.
		 */
		move alphaBetaNegamaxRoot();

		/*
		 * Negamax tree searching method with alpha-beta cutoff.
		 * - depth left: Current search depth. If this is 0, then we dont go deeper and evaluate.
		 * - game info: the non-reversible elements of the board. These are copy made and passed in.
		 * - pvLine: collected principal variation of the parent node.
		 */
		int alphaBetaNegamax(int alpha, int beta, unsigned int depthLeft, game_information& gameInfo);
	};

}

#endif /* SRC_ENGINE_ENGINE_H_ */
