#include <iostream>
#include <unordered_map>
#include "../include/bitboards.h"
#include "../include/prints.h"





// int get_first_square(uint64_t bitboard) {
//     int count = 0;
//     while (bitboard > 0) {
//         if (bitboard & 1) {  // Check if the least significant bit is set
//             break;
//         }
//         bitboard >>= 1;  // Right shift the number by 1 bit
//         count++;
//     }
    
//     return count;
// }

// int count_bits(uint64_t bitboard){
//     // bit counter
//     int count = 0;
    
//     // consecutively reset least significant 1st bit
//     while (bitboard)
//     {
//         // increment count
//         count++;
        
//         // reset least significant 1st bit
//         bitboard &= bitboard - 1;
//     }
    
//     // return bit count
//     return count;
// }

uint64_t generate_occupancy(int decimal_pattern, int attack_bits_number, uint64_t attack_mask){
    // occupancy map - discovering the occupany pattern in binary
    /*
    ex:
    Pattern 0: 00000000000000000
    Pattern 1: 00000000000000001
    Pattern 2: 00000000000000010
    ...
    Pattern 131,071: 11111111111111110
    Pattern 131,072: 11111111111111111
    
    index = pattern which corresponds to a decimal number
    */

    uint64_t occupancy = 0ULL;
    
    // loop over the range of bits within attack mask
    for (int count = 0; count < attack_bits_number; count++){
        // get LS1B index of attacks mask
        int square = get_first_square(attack_mask);
        
        // the bit in the pattern is either 0 or 1
        // make sure the bit aligns with the pattern to create the pattern (aka index or decimal_pattern)
        if (decimal_pattern & (1 << count)){
            PUT_BIT(occupancy , square);
        }

        // pop least first bit in attack map
        DEL_BIT(attack_mask, square);
        
    }
    
    // return occupancy map
    return occupancy;
}


inline void innerfen(game_data& gd){
    // uint64_t rookbitboard = gd.bitboards[2];
    // uint64_t kingbitboard = gd.bitboards[0];
    // int r1 = get_first_square(rookbitboard);
    // DEL_BIT(rookbitboard,r1);
    // int r2 = get_first_square(rookbitboard); // cant be more than two rooks and 1 king for each sides in initial position
    // int k = get_first_square(kingbitboard);
    gd.wr1 = INVALID;
    gd.wr2 = INVALID;
    gd.br1 = INVALID;
    gd.br2 = INVALID;
    gd.wk  = INVALID;
    gd.bk  = INVALID;

    // gd.k = k;
    gd.wk = get_first_square(gd.bitboards[0]);

    if( 1 & gd.bitboards[2]){
        gd.wr1 = 0;
    }else{
        for( int r=0; r<gd.wk; r++){
            if((1ULL << r) & gd.bitboards[2]){
                gd.wr1 = r;
                break;
            }
        }
    }

    if((1ULL << 7) & gd.bitboards[2]){
        gd.wr2 = 7;
    }else{
        for(int r=gd.wk+1; r<8; r++){
            if((1ULL << r) & gd.bitboards[2]){
                gd.wr2 = r;
            }
        }
    }


    gd.bk = get_first_square(gd.bitboards[6]);

    if( (1ULL << 56) & gd.bitboards[8]){
        gd.br1 = 56;
    }else{
        for( int r=56; r<gd.bk; r++){
            if((1ULL << r) & gd.bitboards[8]){
                gd.br1 = r;
                break;
            }
        }
    }

    if((1ULL << 63) & gd.bitboards[8]){
        gd.br2 = 63;
    }else{
        for(int r=gd.bk+1; r<64; r++){
            if((1ULL << r) & gd.bitboards[8]){
                gd.br2 = r;
            }
        }
    }
    if(gd.wr1 != INVALID){
        gd.castling_rights[gd.wr1] = 14; //13 // 11 // 7
    }

    if(gd.wr2 != INVALID){
        gd.castling_rights[gd.wr2] = 13; //13 // 11 // 7
    }

    if(gd.br1 != INVALID){
        gd.castling_rights[gd.br1] = 11; //13 // 11 // 7
    }

    if(gd.br2 != INVALID){
        gd.castling_rights[gd.br2] = 7; //13 // 11 // 7
    }

    if(gd.wk != INVALID){
        gd.castling_rights[gd.wk] = 12; //13 // 11 // 7
    }
    
    if(gd.bk != INVALID){
        gd.castling_rights[gd.bk] = 3; //13 // 11 // 7
    }
    gd.wking_sqr = gd.wk;
    gd.bking_sqr = gd.bk;
};

game_data parse_fen(const char* fen){ // 4k2r/6r1/8/8/8/8/3R4/R3K3 w kQ - 0 1 /  rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1  0=halfmvoe
                                      // 6kr/6r1/8/8/8/8/3R4/R3K3 w kQ - 0 1
    game_data gd;
    int sq=0;
    int file=0;
    int rank=8;
    std::unordered_map<std::string, int> castlingValues = {
        {"-", 0},       // 0000
        {"K", 1},       // 0001
        {"Q", 2},       // 0010
        {"KQ", 3},      // 0011
        {"k", 4},       // 0100
        {"Kk",5},       // 0101
        {"kQ", 6},      // 0110
        {"KQk",7},      // 0111
        {"q", 8},       // 1000
        {"Kq",9},       // 1001
        {"Qq",10},      // 1010
        {"KQq",11},     // 1011
        {"kq", 12},     // 1100
        {"Kkq", 13},    // 1101
        {"Qkq", 14},    // 1110
        {"KQkq", 15},   // 1111
    };
    //char char_pieces[12] ={'K','Q','R','B','N','P','k','q','r','b','n','p'};
 
    while (*fen != '\0') {

        sq = (8 * rank) - file - 1;
        if(*fen==' '){
            //state='0';
            std::string fenString = std::string(fen);

            std::size_t firstpos = fenString.find(' ');
            std::size_t secondpos = fenString.find(' ',firstpos + 1);
            std::size_t thirdpos = fenString.find(' ',secondpos + 1);
            std::size_t forthpos = fenString.find(' ',thirdpos + 1);
            std::size_t fifthpos = fenString.find(' ',forthpos + 1);

            if (firstpos != std::string::npos && secondpos != std::string::npos &&
                thirdpos != std::string::npos && forthpos != std::string::npos &&
                fifthpos != std::string::npos) {
                // Process the fields based on the positions of the spaces
                
                std::string right_move = fenString.substr(firstpos + 1, secondpos - firstpos - 1);
                std::string castles = fenString.substr(secondpos + 1, thirdpos - secondpos - 1);
                std::string en_passant = fenString.substr(thirdpos + 1, forthpos - thirdpos - 1);
                std::string halfmove = fenString.substr(forthpos + 1, fifthpos - forthpos - 1);
                std::string fullmove = fenString.substr(fifthpos + 1);

                gd.castles=castlingValues[castles];

                
                if (en_passant.size() == 2) {
                    int file = 7  - (en_passant[0] - 'a');
                    int rank = (en_passant[1] - '0') - 1;
                    if (file >= 0 && file < 8 && rank >= 0 && rank < 8) {
                        gd.en_passant = rank * 8 + file;
                    }
                }
                    
                gd.halfmove = std::stoi(halfmove);
                gd.fullmove = std::stoi(fullmove);
                gd.side_to_move = right_move == "w" ? 0 : 1;
                
            } else {
                // Handle the case where the FEN string is not in the expected format
                // ...
            }
            innerfen(gd);
            return gd;
            
        }else if(*fen=='/'){
            rank--;
            file=-1;
        }else if(isdigit(*fen)){
         
            file += (*fen-'0' - 1);
        }else{
            int index= str_pieces.find(*fen);
            
            if(index != std::string::npos) {
                gd.piece_numbers[index]++;
                gd.mailbox[sq] = (uint8_t)index;
                gd.bitboards[index] |= (1ULL << sq);
                gd.occupancy[2] |= (1ULL << sq);

                if(str_white.find(*fen) !=std::string::npos){
                    gd.occupancy[0] |= (1ULL << sq);
                }else if(str_black.find(*fen) !=std::string::npos){
                    gd.occupancy[1] |= (1ULL << sq);
                }
            }else { //error
                    
            }
        }
        file++;
        fen++;
    }
    innerfen(gd);
    return gd;
}