#pragma once
#include "game.h"
#include "evaluation.h"

// Blackpiexel is a an engine that wraps all the components with a main functionality of controling the flow.

// 1 - the main entry for webassembly method. converting input and output for the components
// 2 - parsing PGNs and converting UCI to PGN and vice versa
// 3 - creating fen
// 4 - PGN parser
// 5 - time and depth management
// 6 - threading


inline Game old_game(STANDARD_POSITION,4);
inline Game game(STANDARD_POSITION,4);

int searchMove(int depth , int greediness, int aggression,int naivity , MoveList& ml, int i);
const char* Blackpixel_engine(const char* input, int depth , int greediness, int aggression,int naivity, int numWorkers = 1,int workerid = 0);
const char* costume_memory_allocation();