#pragma once
#include <string>
#include "globals.h"

class Game {
public:
    Game(const std::string& fen, int depth); 
    float eval();
    std::string best_move();
    void move(const std::string& move);
    std::string get_fen();
    game_data get_gd();

private:
   int _depth;
   std::string _fen;
   game_data _gd; 
};
