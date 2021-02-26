#include "human_player_gui.h"

namespace tchess
{
    move human_player_gui::makeMove(game* gameController)
    {
        return move();
    }

    std::string human_player_gui::description() const
    {
        return "User controlled player";
    }
}
