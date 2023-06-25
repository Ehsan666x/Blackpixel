#include <string>
#include <iostream>
#include <cstring>

#include "../include/globals.h"
#include "../include/bitboards.h"
#include "../include/prints.h"




//int ascii_pieces[12] = {'♔', '♕', '♖', '♗', '♘', '♙', '♚', '♛', '♜', '♝', '♞', '♟'}; //extern  white and black


void print_bitboard(uint64_t bitboard){
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

void print_chessboard(game_data gd){
    std::string board[64];
    //memset(board, ".", sizeof(board));
    for (int i = 0; i < 64; i++) {
        board[i] = ".";
    }
    for(int i=0;i<12;i++){
        if(gd.bitboards[i]){
            for(int r=0;r<8;r++){
                for(int f=0;f<8;f++){
                    int sq = (8 * r) + f;
                    if(gd.bitboards[i] & (1ULL << sq)){
                        board[sq]=utf8_pieces[i];
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
