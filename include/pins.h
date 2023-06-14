

inline std::unordered_map<int, std::unordered_map <int, std::unordered_map<uint64_t, uint64_t> > > direction_attack_tables; // [direction][sqr][occupancy]=mask

uint64_t sliding_pins_manually(int direction,int sqr, uint64_t& block_mask);

enum directions {
    DIAG_RT, // diagonal right top to left bottom
    DIAG_RB, // diagonal right bottom  left top
    DIAG_LB, // diagonal left bottom to  right top 
    DIAG_LT, // diagonal left top to right bottom
    HORIZ_L, // horizontal left to right
    HORIZ_R, // horizontal right to left
    VERTIC_T, // vertical top to bottom
    VERTIC_B, // vertical bottom to top

};

uint64_t pin_mask(game_data& gd);