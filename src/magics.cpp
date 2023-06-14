#include <iostream>
#include <cstring>
#include "../include/bitboards.h"
#include "../include/attacks.h"
#include "../include/magics.h"
#include "../include/prints.h"




namespace Magicnumber{

    // pseudo random number state
    unsigned int random_state = 1804289383;

    // generate 32-bit pseudo legal numbers
    unsigned int get_random_U32_number(){
        // get current state
        unsigned int number = random_state;
        
        // XOR shift algorithm
        number ^= number << 13;
        number ^= number >> 17;
        number ^= number << 5;
        
        // update random number state
        random_state = number;
        
        // return random number
        return number;
    };

    // generate 64-bit pseudo legal numbers
    uint64_t get_random_U64_number(){
        // define 4 random numbers
        uint64_t n1, n2, n3, n4;
        
        // init random numbers slicing 16 bits from MS1B side
        n1 = (uint64_t)(get_random_U32_number()) & 0xFFFF;
        n2 = (uint64_t)(get_random_U32_number()) & 0xFFFF;
        n3 = (uint64_t)(get_random_U32_number()) & 0xFFFF;
        n4 = (uint64_t)(get_random_U32_number()) & 0xFFFF;
        
        // return random number
        return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
    };

    // generate magic number candidate
    uint64_t generate(){
        return get_random_U64_number() & get_random_U64_number() & get_random_U64_number();
    }    

}

void init_magic_tables(){
    if(!magic_tables[0][0]){
        for (int sqr = 0; sqr < 64; sqr++){
            for(int p=0; p<2; p++){
                int relevant_bits = relevant_array[p][sqr];
                // init occupancies
                uint64_t occupancies[4096];

                // init attack tables
                uint64_t attacks[4096];

                // init used attacks
                uint64_t used_attacks[4096];

                // init attack mask for a current piece
                //uint64_t attack_mask = bishop ? mask_bishop_attacks(square) : mask_rook_attacks(square);
                uint64_t attack_mask = raw_attacks[p? 2:3][sqr];
                //uint64_t rook_raw_attack = raw_attacks[p+2][sqr];

                // init occupancy indicies
                uint64_t occupancy_indicies = 1 << relevant_bits;

                // loop over occupancy indicies
                for (uint64_t index = 0; index < occupancy_indicies; index++){

                    // init occupancies
                    occupancies[index] = generate_occupancy(index, relevant_bits, attack_mask);
                    
                    attacks[index] = sliding_attacks_manually(p ,sqr, occupancies[index] );
                }

                // test magic numbers loop
                for (uint64_t random_count = 0; random_count < 100000000; random_count++){

                    // generate magic number candidate
                    uint64_t magic_number = Magicnumber::generate();
                    
                    // skip inappropriate magic numbers
                    if (count_bits((attack_mask * magic_number) & 0xFF00000000000000) < 6) continue;
                    
                    // init used attacks
                    memset(used_attacks, 0ULL, sizeof(used_attacks));
                    
                    // init index & fail flag
                    uint64_t index, fail;
                    
                    // test magic index loop
                    for (index = 0, fail = 0; !fail && index < occupancy_indicies; index++){
                    
                        // init magic index
                        uint64_t magic_index = ((occupancies[index] * magic_number) >> (64 - relevant_bits));
                        
                        // if magic index works
                        if (used_attacks[magic_index] == 0ULL)
                            // init used attacks
                            used_attacks[magic_index] = attacks[index];
                        
                        // otherwise
                        else if (used_attacks[magic_index] != attacks[index])
                            // magic index doesn't work
                            fail = 1;
                    }
                    
                    // if magic number works
                    if (!fail){
                        // return it
                        //return magic_number;
                            // loop over 64 board squares
                        
                            //magic_tables[p][sqr] = magic_number;
                            std::cout << magic_number << "ULL,"<< std::endl;
                            //log(magic_number);
                            break;
                            //rook_magic_numbers[sqr] = find_magic_number(sqr, rook_relevant_bits[sqr], rook);

                            //bishop_magic_numbers[sqr] = find_magic_number(sqr, bishop_relevant_bits[sqr], bishop);
                        
                    }
                        

                }
            }
        
        }

    }
  
};






