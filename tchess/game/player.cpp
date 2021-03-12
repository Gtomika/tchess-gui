#include <fstream>

#include "player.h"

tchess::saved_game::saved_game(std::string wpn, std::string bpn, std::string res, std::string rea,
	const std::vector<move>& moves, const std::vector<move_extra_info> extras)
	: whitePlayer(wpn), blackPlayer(bpn), result(res), reason(rea),
	moves(moves), moveExtras(extras)
{

}

void tchess::saved_game::writeToFile(CString p)
{
	CT2CA convertedPath(p);
	std::string path(convertedPath);

	std::ofstream output;
	output.open(path);

	output << whitePlayer << std::endl;
	output << blackPlayer << std::endl;
	output << result << std::endl;
	output << reason << std::endl;

	for (size_t i = 0; i < moves.size(); ++i) {
		move_extra_info& extra = moveExtras[i];
		move& m = moves[i];
		std::string moveString = m.to_string(extra.pieceThatMoved) + " " + extra.appendToMove;
		output << moveString;
		if (i < moves.size() - 1) {
			output << std::endl;
		}
	}

	output.close();
}

tchess::saved_game tchess::readSaveFromFile(CString p)
{
	CT2CA convertedPath(p);
	std::string path(convertedPath);

	std::ifstream input;
	input.open(path);

	std::string whitePlayer, blackPlayer, result, reason;
	std::vector<move> moves;

	UINT lineCount = 1;
	UINT side = white;
	for (std::string line; getline(input, line); )
	{
		/*
		CString message;
		message.Format(_T("Line %d: %s"), lineCount, CString(line.c_str()).GetBuffer());
		AfxMessageBox(message, MB_OK);
		*/
		if (lineCount == 1) {
			whitePlayer = line;
		}
		else if (lineCount == 2) {
			blackPlayer = line;
		}
		else if (lineCount == 3) {
			result = line;
		}
		else if (lineCount == 4) {
			reason = line;
		}
		else { //moves
			moves.push_back(parse_move(line, side)); //can throw move parse exception
			side = 1 - side;
		}
		++lineCount;
	}
	input.close();
	if (reason == "Resignation") {
		//if the player resigned in the saved game, then the last move is resignation
		moves.push_back(move(0, 0, resignMove, 0));
	}
	//move extra info will be rebuilt the the game controller anyways when playing out the moves
	return saved_game(whitePlayer, blackPlayer, result, reason,
		moves, std::vector<move_extra_info>());
}

tchess::saved_game_player::saved_game_player(UINT side, const std::vector<move>& moves,
	const std::string& n) : player(side, false, nullptr), name(n)
{
	//push moves into the stack IN REVERSE order
	for (int i = moves.size() - 1; i >= 0; --i) {
		moveStack.push(moves[i]);
	}

}