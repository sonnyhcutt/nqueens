#include "board_io.h"

#include <fstream>
#include <iostream>

bool saveBoard(const CharBoard& board, const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile) {
        std::cerr << "Error opening file for saving.\n";
        return false;
    }

    outFile << board.size() << '\n';
    for (const auto& row : board) {
        for (char cell : row) {
            outFile << cell;
        }
        outFile << '\n';
    }

    std::cout << "Board saved to " << filename << '\n';
    return true;
}

CharBoard loadBoard(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile) {
        std::cerr << "Error opening file for loading.\n";
        return {};
    }

    int size = 0;
    if (!(inFile >> size) || size <= 0 || size > kMaxQueensBoardSize) {
        std::cerr << "Invalid board size in " << filename << ".\n";
        return {};
    }

    CharBoard board(size, std::vector<char>(size, '.'));
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            char cell;
            if (!(inFile >> cell) || (cell != '.' && cell != 'Q')) {
                std::cerr << "Malformed board data in " << filename << ".\n";
                return {};
            }
            board[i][j] = cell;
        }
    }

    std::cout << "Board loaded from " << filename << '\n';
    return board;
}
