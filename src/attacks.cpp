#include "../include/attacks.h"
#include "../include/globals.h"
#include "../include/bitboards.h"
#include "../include/magics.h"


void init_raw_attacks(){

    // full_Square = 0ULL;
    // empty_a = 0ULL;
    // empty_h = 0ULL;
    // empty_gh = 0ULL;
    // empty_ab = 0ULL;
    for(int i=0;i<64;i++){
        PUT_BIT(full_Square,i);
    }
    for(int rank=0;rank<8;rank++){
        for(int file=0;file<8;file++){
            int sqr = (rank * 8) + file;
            if(file!=0){
                PUT_BIT(empty_h,sqr);
            }
            if(file!=0 && file!=1){
                PUT_BIT(empty_gh,sqr);
            }
            if(file!=7){
                PUT_BIT(empty_a,sqr);
            }
             if(file!=7 && file!=6){
                PUT_BIT(empty_ab,sqr);
            }
        }
    }
    for(int i=0; i<7 ; i++){ // k q r b n P p
        for(int sqr=0;sqr<64;sqr++){
            uint64_t attackbits = 0ULL;
            uint64_t piecebit = 0ULL;
            PUT_BIT(piecebit,sqr);

            if(i==0){ // king

                if (piecebit >> 8){ attackbits |= (piecebit >> 8);}
                if ((piecebit >> 9) & empty_a){ attackbits |= (piecebit >> 9);}
                if ((piecebit >> 7) & empty_h){ attackbits |= (piecebit >> 7);}
                if ((piecebit >> 1) & empty_a){ attackbits |= (piecebit >> 1);}
                if (piecebit << 8){ attackbits |= (piecebit << 8);}
                if ((piecebit << 9) & empty_h){ attackbits |= (piecebit << 9);}
                if ((piecebit << 7) & empty_a){ attackbits |= (piecebit << 7);}
                if ((piecebit << 1) & empty_h){ attackbits |= (piecebit << 1);}
                
            }else if(i==1){ // queen
                int rank = sqr / 8;
                int file = sqr % 8;
                
                for (int r = rank + 1; r < 7; r++){ PUT_BIT(attackbits,(r*8 + file));}
                for (int f = file + 1; f < 7; f++){ PUT_BIT(attackbits,(rank*8 + f));}
                for (int r = rank - 1; r > 0; r--){ PUT_BIT(attackbits,(r*8 + file));}
                for (int f = file - 1; f > 0; f--){ PUT_BIT(attackbits,(rank*8 + f));}
                for (int r = rank + 1, f = file + 1; r < 7 && f < 7; r++, f++){ PUT_BIT(attackbits,(r*8+f));}
                for (int r = rank - 1, f = file + 1; r > 0 && f < 7; r--, f++){ PUT_BIT(attackbits,(r*8+f));}
                for (int r = rank + 1, f = file - 1; r < 7 && f > 0; r++, f--){ PUT_BIT(attackbits,(r*8+f));}
                for (int r = rank - 1, f = file - 1; r > 0 && f > 0; r--, f--){ PUT_BIT(attackbits,(r*8+f));}

            }else if(i==2){ // rook
                int rank = sqr / 8;
                int file = sqr % 8;
                
                
                for (int r = rank + 1; r < 7; r++){ PUT_BIT(attackbits,(r*8 + file));}
                for (int f = file + 1; f < 7; f++){ PUT_BIT(attackbits,(rank*8 + f));}
                for (int r = rank - 1; r > 0; r--){ PUT_BIT(attackbits,(r*8 + file));}
                for (int f = file - 1; f > 0; f--){ PUT_BIT(attackbits,(rank*8 + f));}
                

            }else if(i==3){ // bishop
                int rank = sqr / 8;
                int file = sqr % 8;
                //for(int r = rank + 1, int f = file + 1; r < )
                for (int r = rank + 1, f = file + 1; r < 7 && f < 7; r++, f++){ PUT_BIT(attackbits,(r*8+f));}
                for (int r = rank - 1, f = file + 1; r > 0 && f < 7; r--, f++){ PUT_BIT(attackbits,(r*8+f));}
                for (int r = rank + 1, f = file - 1; r < 7 && f > 0; r++, f--){ PUT_BIT(attackbits,(r*8+f));}
                for (int r = rank - 1, f = file - 1; r > 0 && f > 0; r--, f--){ PUT_BIT(attackbits,(r*8+f));}
                
            }else if(i==4){ // knight

        
                if ((piecebit >> 17) & empty_a){ attackbits |= (piecebit >> 17);}
                if ((piecebit >> 15) & empty_h){ attackbits |= (piecebit >> 15);}
                if ((piecebit >> 10) & empty_ab){ attackbits |= (piecebit >> 10);}
                if ((piecebit >> 6) & empty_gh){ attackbits |= (piecebit >> 6);}
                if ((piecebit << 17) & empty_h){ attackbits |= (piecebit << 17);}
                if ((piecebit << 15) & empty_a){ attackbits |= (piecebit << 15);}
                if ((piecebit << 10) & empty_gh){ attackbits |= (piecebit << 10);}
                if ((piecebit << 6) & empty_ab){ attackbits |= (piecebit << 6);}
            
            }else if(i==5){ // white pawn
                if ((piecebit << 7) & empty_a){ attackbits |= (piecebit << 7);}
                if ((piecebit << 9) & empty_h){ attackbits |= (piecebit << 9);}  

            }else if(i==6){ // black pawn
                if ((piecebit >> 7) & empty_h){ attackbits |= (piecebit >> 7);}
                if ((piecebit >> 9) & empty_a){ attackbits |= (piecebit >> 9);}
            }
            raw_attacks[i][sqr] = attackbits;
        }
    }
}


inline uint64_t sliding_attacks_manually(int p,int sqr, uint64_t& block_mask){
    uint64_t attackbits = 0ULL;
    int rank = sqr / 8;
    int file = sqr % 8;
    if(!p){ // bishop
        for (int r = rank + 1, f = file + 1; r < 8 && f < 8; r++, f++){ PUT_BIT(attackbits,(r*8+f)); if ((1ULL << (r * 8 + f)) & block_mask) break;}
        for (int r = rank - 1, f = file + 1; r >= 0 && f < 8; r--, f++){ PUT_BIT(attackbits,(r*8+f));if ((1ULL << (r * 8 + f)) & block_mask) break;}
        for (int r = rank + 1, f = file - 1; r < 8 && f >= 0; r++, f--){ PUT_BIT(attackbits,(r*8+f));if ((1ULL << (r * 8 + f)) & block_mask) break;}
        for (int r = rank - 1, f = file - 1; r >= 0 && f >= 0; r--, f--){ PUT_BIT(attackbits,(r*8+f));if ((1ULL << (r * 8 + f)) & block_mask) break;}
    }else if(p==1){ // rook
         
        for (int r = rank + 1; r < 8; r++){ PUT_BIT(attackbits,(r*8 + file));if ((1ULL << (r*8 + file)) & block_mask) break;}
        for (int f = file + 1; f < 8; f++){ PUT_BIT(attackbits,(rank*8 + f));if ((1ULL << (rank*8 + f)) & block_mask) break;}
        for (int r = rank - 1; r >= 0; r--){ PUT_BIT(attackbits,(r*8 + file));if ((1ULL << (r*8 + file)) & block_mask) break;}
        for (int f = file - 1; f >= 0; f--){ PUT_BIT(attackbits,(rank*8 + f));if ((1ULL << (rank*8 + f)) & block_mask) break;}
    }
    return attackbits;
}

void init_sliding_attacks_tables(){
    for(int sqr=0;sqr<64;sqr++){
        for(int p=0;p<2;p++){
            int attack_mask = raw_attacks[!p? 3 : 2][sqr];
            int attack_bits_number = count_bits(attack_mask);
            int occupancy_indicies = (1 << attack_bits_number);
            for (int index = 0; index < occupancy_indicies; index++){

                uint64_t occupancy = generate_occupancy(index, attack_bits_number, attack_mask);
                
                // init magic index
                int magic_index = (occupancy * magic_tables[p][sqr]) >> (64 - relevant_array[p][sqr]);
                
                // init bishop attacks
                sliding_attack_tables[p][sqr][magic_index] = sliding_attacks_manually(p,sqr, occupancy);
                //int sa = sliding_attacks_manually(p,sqr, occupancy);
            }

        }
    }
}

uint64_t get_sliding_attacks(int p,int sqr, uint64_t occupancy){
    
    int attack_mask = raw_attacks[p==3 || p==0 ? 3: 2][sqr];
    occupancy &= attack_mask;
    int magic_index = (occupancy * magic_tables[p==3 || p==0 ? 0: 1][sqr]) >> (64 - relevant_array[p==3 || p==0 ? 0: 1][sqr]);
    
    return sliding_attack_tables[p==3 || p==0 ? 0: 1][sqr][magic_index];
}
