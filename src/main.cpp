#include <iostream>
#include "../include/funcs.h"


namespace{
    void log(auto& message) {
        std::cout << message << std::endl;
    }
}

int main(){
    int depth;
    std::string input;
    log("Enter your fen");
    // //std::cout << "Enter your fen" << std::endl;
    std::cin >> input;
    log("Enter Depth");
    // //std::cout << "Enter Depth" << std::endl;
    std::cin >> depth;
    auto out = inout(input, (int)depth);
    std::string message = "Best move :" + out.bm + " " + "Evaluation :" + std::to_string(out.e);
    log(message);
    return 0;
}
