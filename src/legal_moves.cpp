#include <iostream>
#include "../include/prints.h"
#include "../include/magics.h"
#include "../include/attacks.h"
#include "../include/globals.h"



uint64_t refine_own_occupency_attack(int& kind , int& sqr,uint64_t& attack ,uint64_t& occupancy){
    
    
    return 0;
}

uint64_t refine_opponent_occupency_attack(int& kind , int& sqr ,uint64_t& attack ,uint64_t& occupancy){
    

    return 0;
}

uint64_t sqr_legal_moves(int& sqr,game_data& gd){
    if(! (gd.occupancy[gd.side_to_move] & (1ULL << sqr) ) ){ // checking if a right piece exists

        return 0;
    }
    int index=0; int i=0;
    if(!gd.side_to_move){
        index=6;
        i=0; 
    }else if(gd.side_to_move==1){
        index=12;
        i=6;
    }
    for( i ; i<index; i++){
        if(gd.bitboards[i] & (1ULL << sqr) ){ // found the piece kind
            int op=0;
            if(!gd.side_to_move){op=1;}
            //uint64_t raw_attack = raw_attacks[i][sqr];
            uint64_t lm = 0ULL;
            //lm = refine_own_occupency_attack(i , sqr ,raw_attack,gd.occupancy[gd.side_to_move]); // i = piece kind 
            lm = refine_own_occupency_attack(i , sqr ,lm, gd.occupancy[op]);

            // other logics (in check / pin)

            return 1ULL;
        }
    }
    return 0;
}


