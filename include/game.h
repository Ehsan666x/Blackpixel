#pragma once
#include <string>

class Game {
public:
    Game(const std::string& fen, int depth); 
    float eval();
    std::string best_move();
    void move(const std::string& move);
    std::string get_fen();

private:
   int _depth;
   std::string _fen; 
};
