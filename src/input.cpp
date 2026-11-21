#include "input.h"

#include <cstdlib>
#include <iostream>
#include <limits>

namespace {

void exitOnEof() {
    if (std::cin.eof()) {
        std::cout << "\nInput closed. Goodbye!" << std::endl;
        std::exit(0);
    }
}

void discardLine() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

} // namespace

int readInt(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        int value;
        if (std::cin >> value) {
            return value;
        }
        exitOnEof();
        discardLine();
        std::cout << "Please enter a whole number.\n";
    }
}

char readChar(const std::string& prompt) {
    // Read a whole word so stray characters (e.g. "yes") don't leak into the next prompt.
    return readWord(prompt)[0];
}

std::string readWord(const std::string& prompt) {
    std::cout << prompt;
    std::string value;
    if (!(std::cin >> value)) {
        exitOnEof();
    }
    return value;
}

bool askYesNo(const std::string& prompt) {
    char answer = readChar(prompt);
    return answer == 'y' || answer == 'Y';
}
