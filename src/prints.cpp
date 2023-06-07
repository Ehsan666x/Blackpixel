#include <string>
#include <iostream>
#include <cstring>

#include "../include/prints.h"
#include "../include/globals.h"
#include "../include/bitboards.h"

std::string notations[64] = { // global extern
    "h1", "g1", "f1", "e1", "d1", "c1", "b1", "a1", 
    "h2", "g2", "f2", "e2", "d2", "c2", "b2", "a2", 
    "h3", "g3", "f3", "e3", "d3", "c3", "b3", "a3", 
    "h4", "g4", "f4", "e4", "d4", "c4", "b4", "a4", 
    "h5", "g5", "f5", "e5", "d5", "c5", "b5", "a5", 
    "h6", "g6", "f6", "e6", "d6", "c6", "b6", "a6", 
    "h7", "g7", "f7", "e7", "d7", "c7", "b7", "a7", 
    "h8", "g8", "f8", "e8", "d8", "c8", "b8", "a8",
};


//int ascii_pieces[12] = {'♔', '♕', '♖', '♗', '♘', '♙', '♚', '♛', '♜', '♝', '♞', '♟'}; //extern  white and black


void print_bitboard(uint64_t& bitboard){
    int sq=0;
 
    for(int r=8 ; r>-1; r--){
        for(int f=0 ;f<9; f++ ){
            sq = (8 * r) - f;
            if(!f && r){
                std::cout << r<<"   ";
            }else if(!r && f){
                std::cout << notations[8-f][0] << "  ";
            }else if(r && f){
                int bit = (bitboard & (1ULL << sq)) ? 1 : 0;
                std::cout << bit << "  ";
            }else{
                std::cout << "    ";
            }
        }
        std::cout <<"\n"<< std::endl;
    }
    std::cout <<"\n\n"<< std::endl;
}

void print_chessboard(game_data& gd){
    char board[64];
    memset(board, '*', sizeof(board));
    for(int i=0;i<12;i++){
        if(gd.bitboards[i]){
            for(int r=0;r<8;r++){
                for(int f=0;f<8;f++){
                    int sq = (8 * r) + f;
                    if(gd.bitboards[i] & (1ULL << sq)){
                        board[sq]=str_pieces[i];
                    }
                }
            }
        }
    }

    int sq=0;
    for(int r=8 ; r>-1; r--){
        for(int f=0 ;f<9; f++ ){
            sq = (8 * r) - f;
            if(!f && r){
                std::cout << r<<"   ";
            }else if(!r && f){
                std::cout << notations[8-f][0] << "  ";
            }else if(r && f){
                std::cout<< board[sq] << "  ";
                
            }else{
                std::cout << "    ";
            }
        }
        std::cout <<"\n"<< std::endl;
    }
    std::cout <<"\n\n"<< std::endl;
    
};
