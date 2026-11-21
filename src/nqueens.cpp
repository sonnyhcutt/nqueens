#include "nqueens.h"

#include <cctype>
#include <iostream>
#include <random>
#include <string>

#include "board_io.h"
#include "game.h"
#include "input.h"
#include "nqueens_solvers.h"

// ---------------------------------------------------------------------------
// Core logic
// ---------------------------------------------------------------------------

bool checkSolution(const CharBoard& board) {
    const int n = static_cast<int>(board.size());
    std::vector<bool> rowUsed(n, false);
    std::vector<bool> colUsed(n, false);
    std::vector<bool> diagUsed(2 * n, false);     // row - col + n
    std::vector<bool> antiDiagUsed(2 * n, false); // row + col
    int queens = 0;

    for (int row = 0; row < n; ++row) {
        for (int col = 0; col < n; ++col) {
            if (board[row][col] != 'Q') continue;
            const int d = row - col + n;
            const int a = row + col;
            if (rowUsed[row] || colUsed[col] || diagUsed[d] || antiDiagUsed[a]) {
                return false;
            }
            rowUsed[row] = colUsed[col] = diagUsed[d] = antiDiagUsed[a] = true;
            ++queens;
        }
    }
    return queens == n;
}

bool solveNQueens(IntBoard& board) {
    const SolveResult result = solveBacktracking(static_cast<int>(board.size()));
    if (!result.solved) return false;
    for (std::size_t row = 0; row < result.queens.size(); ++row) {
        board[row][result.queens[row]] = 1;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Display / interaction
// ---------------------------------------------------------------------------

// Columns are labeled a, b, c...; rows 1, 2, 3... (e.g. "b3").
void printPlayerBoard(const CharBoard& board) {
    const int n = static_cast<int>(board.size());

    std::cout << "   ";
    for (int file = 0; file < n; ++file) {
        std::cout << static_cast<char>('a' + file) << ' ';
    }
    std::cout << '\n';

    for (int rank = 0; rank < n; ++rank) {
        std::cout << (rank + 1 < 10 ? " " : "") << rank + 1 << ' ';
        for (int file = 0; file < n; ++file) {
            std::cout << board[rank][file] << ' ';
        }
        std::cout << '\n';
    }
}

void printSolutionBoard(const IntBoard& board) {
    for (const auto& row : board) {
        for (int cell : row) {
            std::cout << (cell == 1 ? "Q " : ". ");
        }
        std::cout << '\n';
    }
}

namespace {

// Parses a square like "a1" or "C10" into zero-based (row, col).
bool parseSquare(const std::string& text, int& row, int& col) {
    if (text.size() < 2 || !std::isalpha(static_cast<unsigned char>(text[0]))) {
        return false;
    }
    col = std::tolower(static_cast<unsigned char>(text[0])) - 'a';
    try {
        size_t consumed = 0;
        row = std::stoi(text.substr(1), &consumed) - 1;
        return consumed == text.size() - 1;
    } catch (...) {
        return false;
    }
}

// Prompts the user to check their board, and shows a correct one if wrong.
void offerSolutionCheck(const CharBoard& board) {
    if (!askYesNo("\nWould you like to check if your solution is correct? (y/n): ")) {
        return;
    }
    if (checkSolution(board)) {
        std::cout << "\nCongratulations! Your solution is correct!\n";
        return;
    }

    std::cout << "\nSorry, your solution is incorrect.\n";
    const int n = static_cast<int>(board.size());
    IntBoard solution(n, std::vector<int>(n, 0));
    if (solveNQueens(solution)) {
        std::cout << "\nHere's a correct solution:\n";
        printSolutionBoard(solution);
    } else {
        std::cout << "\nNo solution exists for this board size.\n";
    }
}

// Plays one round on an empty n x n board and returns the final board.
CharBoard playRound(int n) {
    CharBoard board(n, std::vector<char>(n, '.'));

    std::cout << "\nEmpty Chessboard:\n";
    printPlayerBoard(board);
    std::cout << "You need to place " << n << " queens on the board.\n";

    placeQueensManually(board);
    std::cout << "\nAll " << n << " queens have been placed on the board.\n";

    offerSolutionCheck(board);
    return board;
}

} // namespace

void placeQueensManually(CharBoard& board) {
    const int n = static_cast<int>(board.size());
    int queensPlaced = 0;

    while (queensPlaced < n) {
        std::string square = readWord("\nEnter the position to place the queen (e.g., a1 b3): ");
        int row = 0;
        int col = 0;

        if (parseSquare(square, row, col) && row >= 0 && row < n && col >= 0 && col < n &&
            board[row][col] == '.') {
            board[row][col] = 'Q';
            ++queensPlaced;
        } else {
            std::cout << "\nInvalid position or spot already taken. Please try again.\n";
        }
        std::cout << '\n';
        printPlayerBoard(board);
    }
}

// ---------------------------------------------------------------------------
// Game modes
// ---------------------------------------------------------------------------

// Player chooses n, and can load/save boards.
void queensStandardMode() {
    while (true) {
        if (askYesNo("\nWould you like to load a board? (y/n): ")) {
            CharBoard loaded = loadBoard(readWord("Enter filename to load: "));
            if (!loaded.empty()) {
                printPlayerBoard(loaded);
                offerSolutionCheck(loaded);
            }
        }

        int n = readInt("\nEnter the size of the chessboard (n x n) (Enter -1 to go back to the menu): ");
        if (n == -1) return;

        if (n < 1 || n > kMaxQueensBoardSize) {
            std::cout << "\nInvalid size. Please enter a number between 1 and "
                      << kMaxQueensBoardSize << ".\n";
            continue;
        }

        CharBoard board = playRound(n);

        if (askYesNo("Would you like to save the board? (y/n): ")) {
            saveBoard(board, readWord("Enter filename to save: "));
        }
    }
}

// Board size increases from 1x1 to 8x8.
void queensSequentialMode() {
    for (int n = 1; n <= 8; ++n) {
        std::cout << "\nStarting game with a " << n << "x" << n << " chessboard.\n";
        playRound(n);

        if (n < 8 && !askYesNo("Would you like to continue to the next board size? (y/n): ")) {
            std::cout << "Exiting sequential mode. Goodbye!\n";
            return;
        }
    }
    std::cout << "\nYou've completed every board in sequential mode!\n";
}

// Board size is randomized between 4 and 12.
void queensRandomMode() {
    static std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> dist(4, kMaxQueensBoardSize);
    const int n = dist(rng);

    std::cout << "Randomly selected board size: " << n << "x" << n << '\n';
    playRound(n);
}

void NQueensGame::play() {
    while (true) {
        int mode = readInt(
            "\nSelect a game mode:\n"
            "1. Standard mode\n"
            "2. Sequential mode\n"
            "3. Random mode\n"
            "4. How to play\n"
            "5. Exit\n");

        switch (mode) {
            case 1: queensStandardMode(); break;
            case 2: queensSequentialMode(); break;
            case 3: queensRandomMode(); break;
            case 4:
                std::cout << "\nHow to play: Given an n x n board with n queens, place each "
                             "queen on the board such that no two queens attack each other.\n";
                break;
            case 5: return;
            default: std::cout << "Invalid choice. Please try again.\n";
        }
    }
}
