#include <string>
#include <iostream>
#include "../include/globals.h"
#include "../include/bitboards.h"
#include "../include/moves.h"
#include "../include/game.h"
//#include "new_fen.cpp"

//std::string calculate_new_fen(std::string& fen, const std::string& move);

Game::Game(const std::string& fen, int depth):_fen(fen) {
    // Initialize the game state based on the provided FEN and depth
    _gd = parse_fen(fen.c_str());
}

float Game::eval(){
    // Calculate and return the evaluation of the current game state
    return 0.3f;
}
std::string Game::best_move() {
    // Calculate and return the best move for the current game state
    return "e4";
}
void Game::move(const std::string& move){
    // Update the game state by making the specified move
    // /std::string new_fen = calculate_new_fen(_fen,move);
    //_fen=new_fen;
}
MoveList Game::possible_moves(){
    MoveList ml;
    ml.generate(_gd);
    return ml;
};

std::string Game::get_fen(){
    return _fen;
}
game_data Game::get_gd(){
    return _gd;
}



