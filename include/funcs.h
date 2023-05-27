#pragma once

#include <string>
#include <iostream>
#include <structs.h>

Out inout(std::string& input, int depth); // this function is declared in Entry.cpp
std::string calculate_new_fen(std::string& fen, const std::string& move);

