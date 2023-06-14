#include <iostream>
#include "../include/bitboards.h"
#include "../include/pins.h"
#include "../include/attacks.h"


uint64_t raw_sliding_pins(int direction,int sqr){
    uint64_t attackbits = 0ULL;
    int rank = sqr / 8;
    int file = sqr % 8;
 
    switch (direction){
        case 0:
            /* right top to left bottom */
            for (int r = rank - 1, f = file + 1; r >= 0 && f < 8; r--, f++){ PUT_BIT(attackbits,(r*8+f));}
            break;
        case 1:
            /* right bottom  left top */
            for (int r = rank + 1, f = file + 1; r < 8 && f < 8; r++, f++){ PUT_BIT(attackbits,(r*8+f)); }
            break;
        case 2:
            /* left bottom to right top */
            for (int r = rank + 1, f = file - 1; r < 8 && f >= 0; r++, f--){ PUT_BIT(attackbits,(r*8+f));}
            break;
        case 3:
            /* left top to right bottom */
            for (int r = rank - 1, f = file - 1; r >= 0 && f >= 0; r--, f--){ PUT_BIT(attackbits,(r*8+f));}
            break;
        case 4:
            /* left to right */
            for (int f = file - 1; f >= 0; f--){ PUT_BIT(attackbits,(rank*8 + f));}
            break;
        case 5:
            /* right to left */
            for (int f = file + 1; f < 8; f++){ PUT_BIT(attackbits,(rank*8 + f));}
            break;
        case 6:
            /* top to bottom */
            for (int r = rank - 1; r >= 0; r--){ PUT_BIT(attackbits,(r*8 + file));}
            break;
        case 7:
            /* bottom to top */
            for (int r = rank + 1; r < 8; r++){ PUT_BIT(attackbits,(r*8 + file));}
            break;
    default:
        break;
    }
    return attackbits;
};



uint64_t sliding_pins_manually(int direction,int sqr, uint64_t& block_mask){
    uint64_t attackbits = 0ULL;
    int rank = sqr / 8;
    int file = sqr % 8;
    // if(direction < 4){ // bishop
    //     for (int r = rank + 1, f = file + 1; r < 8 && f < 8; r++, f++){ PUT_BIT(attackbits,(r*8+f)); if ((1ULL << (r * 8 + f)) & block_mask) break;}
    //     for (int r = rank - 1, f = file + 1; r >= 0 && f < 8; r--, f++){ PUT_BIT(attackbits,(r*8+f));if ((1ULL << (r * 8 + f)) & block_mask) break;}
    //     for (int r = rank + 1, f = file - 1; r < 8 && f >= 0; r++, f--){ PUT_BIT(attackbits,(r*8+f));if ((1ULL << (r * 8 + f)) & block_mask) break;}
    //     for (int r = rank - 1, f = file - 1; r >= 0 && f >= 0; r--, f--){ PUT_BIT(attackbits,(r*8+f));if ((1ULL << (r * 8 + f)) & block_mask) break;}
    // }else{ // rook
        
    //     for (int r = rank + 1; r < 8; r++){ PUT_BIT(attackbits,(r*8 + file));if ((1ULL << (r*8 + file)) & block_mask) break;}
    //     for (int f = file + 1; f < 8; f++){ PUT_BIT(attackbits,(rank*8 + f));if ((1ULL << (rank*8 + f)) & block_mask) break;}
    //     for (int r = rank - 1; r >= 0; r--){ PUT_BIT(attackbits,(r*8 + file));if ((1ULL << (r*8 + file)) & block_mask) break;}
    //     for (int f = file - 1; f >= 0; f--){ PUT_BIT(attackbits,(rank*8 + f));if ((1ULL << (rank*8 + f)) & block_mask) break;}
    // }
    switch (direction){
        case 0:
            /* right top to left bottom */
            for (int r = rank - 1, f = file + 1; r >= 0 && f < 8; r--, f++){ PUT_BIT(attackbits,(r*8+f));if ((1ULL << (r * 8 + f)) & block_mask) break;}
            break;
        case 1:
            /* right bottom  left top */
            for (int r = rank + 1, f = file + 1; r < 8 && f < 8; r++, f++){ PUT_BIT(attackbits,(r*8+f)); if ((1ULL << (r * 8 + f)) & block_mask) break;}
            break;
        case 2:
            /* left bottom to right top */
            for (int r = rank + 1, f = file - 1; r < 8 && f >= 0; r++, f--){ PUT_BIT(attackbits,(r*8+f));if ((1ULL << (r * 8 + f)) & block_mask) break;}
            break;
        case 3:
            /* left top to right bottom */
            for (int r = rank - 1, f = file - 1; r >= 0 && f >= 0; r--, f--){ PUT_BIT(attackbits,(r*8+f));if ((1ULL << (r * 8 + f)) & block_mask) break;}
            break;
        case 4:
            /* left to right */
            for (int f = file - 1; f >= 0; f--){ PUT_BIT(attackbits,(rank*8 + f));if ((1ULL << (rank*8 + f)) & block_mask) break;}
            break;
        case 5:
            /* right to left */
            for (int f = file + 1; f < 8; f++){ PUT_BIT(attackbits,(rank*8 + f));if ((1ULL << (rank*8 + f)) & block_mask) break;}
            break;
        case 6:
            /* top to bottom */
            for (int r = rank - 1; r >= 0; r--){ PUT_BIT(attackbits,(r*8 + file));if ((1ULL << (r*8 + file)) & block_mask) break;}
            break;
        case 7:
            /* bottom to top */
            for (int r = rank + 1; r < 8; r++){ PUT_BIT(attackbits,(r*8 + file));if ((1ULL << (r*8 + file)) & block_mask) break;}
            break;
    default:
        break;
    }
    return attackbits;
};



void init_direction_attacks_tables(){
    for(int sqr=0;sqr<64;sqr++){
        for(int direction=0; direction<8; direction++){
            uint64_t attack_mask = raw_sliding_pins(direction ,sqr);
            while (attack_mask){
                int block_sqr = get_first_square(attack_mask);
                uint64_t occupancy = 1ULL << block_sqr;
                direction_attack_tables[direction][sqr][occupancy] = sliding_pins_manually(direction,sqr, occupancy);

                DEL_BIT(attack_mask,block_sqr);
            }

        }
    }
};

uint64_t pin_mask(game_data& gd){
    uint64_t mask =0ULL;

    for(int i=(gd.side_to_move? 1:7); i < (gd.side_to_move? 4:10); i++){ // through sliding bitwises
        
        uint64_t copy_attacker_bitboard = gd.bitboards[i];
        uint64_t opposit_king_sqr = get_first_square(gd.bitboards[gd.side_to_move?1:0]);
        while (copy_attacker_bitboard){
            uint64_t attacker_sqr = get_first_square(copy_attacker_bitboard);
            uint64_t raw_attack_mask = raw_attacks[i][attacker_sqr];
            if(raw_attack_mask & opposit_king_sqr){ // sliding aligned with king

                if((int)opposit_king_sqr/8 == (int)attacker_sqr/8){ //same rank
  
                    mask |= (((1ULL << (opposit_king_sqr>attacker_sqr? (opposit_king_sqr-1) : (attacker_sqr-1) )) - 1) >> (opposit_king_sqr>attacker_sqr?  attacker_sqr : opposit_king_sqr )) << ((opposit_king_sqr>attacker_sqr? attacker_sqr : opposit_king_sqr )-1);
                  
                }else if ((int)opposit_king_sqr%8 == (int)attacker_sqr%8){ // same file


                    for (int rank = std::min((int)opposit_king_sqr/8 , (int)attacker_sqr/8 ) + 1; rank < std::max((int)opposit_king_sqr/8 , (int)attacker_sqr/8); ++rank) {
                        mask |= (1ULL << ((int)opposit_king_sqr%8 + rank * 8));
                    }

                }else{ // diagonal
  
                    if(attacker_sqr > opposit_king_sqr){ // T
                       if(attacker_sqr%8 < opposit_king_sqr%8){ // 0
                           mask |= direction_attack_tables[DIAG_RT][attacker_sqr][1ULL << opposit_king_sqr];
                       }else{ // 3
                           mask |= direction_attack_tables[DIAG_LT][attacker_sqr][1ULL << opposit_king_sqr];
                       }
                    }else{ // B

                       if(attacker_sqr%8 < opposit_king_sqr%8){ //2
                            mask |= direction_attack_tables[DIAG_LB][attacker_sqr][1ULL << opposit_king_sqr];
                       }else{ //1
                            mask |= direction_attack_tables[DIAG_RB][attacker_sqr][1ULL << opposit_king_sqr];
                       }
                    }
                }
            }
            
            DEL_BIT(copy_attacker_bitboard,attacker_sqr);
        };

    }

    return mask;
};

