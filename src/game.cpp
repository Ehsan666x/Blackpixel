//#include <string>
#include <cstring>
#include <iostream>
#include "../include/game.h" // string moves.h
//#include "../include/globals.h"
#include "../include/bitboards.h"
//#include "../include/moves.h"
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
    int src_file = 7  - (move[0] - 'a');
    int src_rank = (move[1] - '0') - 1;

    int target_file = 7 - (move[2] - 'a');
    int target_rank = (move[3] - '0') - 1;

    int sqr = src_rank * 8 + src_file;
    int target = target_rank * 8 + target_file;

    bool valid =false;  
    int piece = 0 , promoted = 0 ,capture = 0 ,double_push = 0 ,enpassant = 0 ,castling = 0;

    MoveList ml = this->possible_moves();
    for( uint32_t m : ml){
        if( (GET_SOURCE_SQUARE(m) == sqr) && (GET_TARGET_SQUARE(m) == target)){
            if(move.length() > 4){
                if(GET_PROMOTED_PIECE(m)){
                    if(promotion_pieces.find(move[4])!=std::string::npos && GET_PROMOTED_PIECE(m) == promotion_pieces.find(move[4])){ // promotion
                        make_move(_gd,m);
                        break;
                    }
                }else if(move[4] == '*' && IS_CASTLING(m)){ // * castle?
                    make_move(_gd,m);
                    break;
                } 
            }else if(!IS_CASTLING(m)){ 
                make_move(_gd,m);
                break;
            }
        }
    }
}

MoveList Game::possible_moves(){
    MoveList ml;
    ml.generate(_gd);
    return ml;
};
ArrayMoveList Game::generate_arraymoves(){
    ArrayMoveList ml;
    ml.generate(_gd);
    return ml;
}

std::string Game::get_fen(){
    return _fen;
}
game_data Game::get_gd(){
    return _gd;
}


//game_data::game_data() = default;
game_data::game_data() {
    for (int i = 0; i < 64; i++) {
        mailbox[i] = INVALID ;
        castling_rights[i] = 15;
    }
}

game_data::game_data(const game_data& other) {
    fen = other.fen;
    memcpy(bitboards, other.bitboards, sizeof(bitboards));
    memcpy(occupancy, other.occupancy, sizeof(occupancy));
    castles = other.castles;
    side_to_move = other.side_to_move;
    en_passant = other.en_passant;
    halfmove = other.halfmove;
    fullmove = other.fullmove;
    wr1 = other.wr1;
    wr2 = other.wr2;
    wk = other.wk;
    br1 = other.br1;
    br2 = other.br2;
    bk = other.bk;
    piece_moves = other.piece_moves;

    wking_sqr = other.wking_sqr;
    bking_sqr = other.bking_sqr;
    for (int i = 0; i < 64; i++) {
        mailbox[i] = other.mailbox[i];
    }
    for(int i=0;i<64;i++){
        castling_rights[i] = other.castling_rights[i];
    }
}

