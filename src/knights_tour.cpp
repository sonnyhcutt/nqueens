#include "knights_tour.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>

#include "game.h"
#include "input.h"

namespace {

constexpr int kKnightMoves[8][2] = {
    {2, 1}, {1, 2}, {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {2, -1},
};

bool solveFrom(KnightBoard& board, int row, int col, int moveNumber) {
    const int n = static_cast<int>(board.size());
    if (moveNumber > n * n) return true;

    for (const auto& move : kKnightMoves) {
        const int nextRow = row + move[0];
        const int nextCol = col + move[1];
        if (isValidKnightMove(nextRow, nextCol, board)) {
            board[nextRow][nextCol] = moveNumber;
            if (solveFrom(board, nextRow, nextCol, moveNumber + 1)) return true;
            board[nextRow][nextCol] = 0; // backtrack
        }
    }
    return false;
}

} // namespace

// ---------------------------------------------------------------------------
// Core logic
// ---------------------------------------------------------------------------

bool isValidKnightMove(int row, int col, const KnightBoard& board) {
    const int n = static_cast<int>(board.size());
    return row >= 0 && row < n && col >= 0 && col < n && board[row][col] == 0;
}

bool isKnightMove(int fromRow, int fromCol, int toRow, int toCol) {
    const int dr = std::abs(toRow - fromRow);
    const int dc = std::abs(toCol - fromCol);
    return (dr == 1 && dc == 2) || (dr == 2 && dc == 1);
}

bool solveKnightsTour(KnightBoard& board, int startRow, int startCol) {
    const int n = static_cast<int>(board.size());
    if (startRow < 0 || startRow >= n || startCol < 0 || startCol >= n) return false;

    board[startRow][startCol] = 1;
    if (solveFrom(board, startRow, startCol, 2)) return true;
    board[startRow][startCol] = 0;
    return false;
}

// ---------------------------------------------------------------------------
// Display / interaction
// ---------------------------------------------------------------------------

// 'K' = knight, '.' = visited, '-' = unvisited.
void displayKnightsBoard(const KnightBoard& board) {
    const int n = static_cast<int>(board.size());

    std::cout << "   ";
    for (int col = 0; col < n; ++col) {
        std::cout << std::setw(2) << col << ' ';
    }
    std::cout << '\n';

    for (int row = 0; row < n; ++row) {
        std::cout << std::setw(2) << row << ' ';
        for (int col = 0; col < n; ++col) {
            if (board[row][col] == -1) {
                std::cout << " K ";
            } else if (board[row][col] > 0) {
                std::cout << " . ";
            } else {
                std::cout << " - ";
            }
        }
        std::cout << '\n';
    }
}

bool playKnightsTour(int boardSize, int startRow, int startCol) {
    KnightBoard board(boardSize, std::vector<int>(boardSize, 0));
    int row = startRow;
    int col = startCol;
    int moveCount = 1;

    board[row][col] = -1;
    displayKnightsBoard(board);

    while (moveCount < boardSize * boardSize) {
        int nextRow = readInt("\nMake your move:\nEnter the row, then the column (-1 -1 to exit): ");
        int nextCol = readInt("");
        if (nextRow == -1) return false;

        if (isKnightMove(row, col, nextRow, nextCol) && isValidKnightMove(nextRow, nextCol, board)) {
            board[row][col] = moveCount++;
            row = nextRow;
            col = nextCol;
            board[row][col] = -1;
            displayKnightsBoard(board);
        } else {
            std::cout << "\nInvalid move. Try again.\n";
        }
    }

    std::cout << "\nCongratulations! You've completed the Knight's Tour!\n";
    return true;
}

namespace {

// Asks for a starting square; returns false if it's off the board.
bool readStartingSquare(int boardSize, int& row, int& col) {
    row = readInt("\nEnter the knight's starting position (row then column, 0 to " +
                  std::to_string(boardSize - 1) + "): ");
    col = readInt("");
    if (row < 0 || row >= boardSize || col < 0 || col >= boardSize) {
        std::cout << "\nInvalid starting position.\n";
        return false;
    }
    return true;
}

} // namespace

// ---------------------------------------------------------------------------
// Game modes
// ---------------------------------------------------------------------------

// Player chooses board size and starting position.
void knightsStandardMode() {
    int boardSize = readInt("\nEnter the size of the chessboard (minimum " +
                            std::to_string(kMinKnightsBoardSize) + "): ");
    if (boardSize < kMinKnightsBoardSize) {
        std::cout << "\nBoard size must be at least " << kMinKnightsBoardSize << "x"
                  << kMinKnightsBoardSize << ".\n";
        return;
    }

    int row = 0;
    int col = 0;
    if (readStartingSquare(boardSize, row, col)) {
        playKnightsTour(boardSize, row, col);
    }
}

// Starts at 5x5 and grows by one after each completed tour.
void knightsSequentialMode() {
    int boardSize = kMinKnightsBoardSize;

    while (true) {
        std::cout << "\nStarting game with a " << boardSize << "x" << boardSize << " chessboard.\n";

        int row = 0;
        int col = 0;
        if (!readStartingSquare(boardSize, row, col)) continue;
        if (!playKnightsTour(boardSize, row, col)) return;

        if (!askYesNo("Would you like to continue to a " + std::to_string(boardSize + 1) + "x" +
                      std::to_string(boardSize + 1) + " board? (y/n): ")) {
            std::cout << "Exiting Knight's Tour Sequential Mode. Goodbye!\n";
            return;
        }
        ++boardSize;
    }
}

// Random starting square on a 5x5 board.
void knightsRandomMode() {
    static std::mt19937 rng{std::random_device{}()};
    const int boardSize = kMinKnightsBoardSize;
    std::uniform_int_distribution<int> dist(0, boardSize - 1);
    const int row = dist(rng);
    const int col = dist(rng);

    std::cout << "Random starting position for the Knight: (" << row << ", " << col << ")\n";
    playKnightsTour(boardSize, row, col);
}

void KnightsGame::play() {
    while (true) {
        int mode = readInt(
            "\nSelect a game mode:\n"
            "1. Standard mode\n"
            "2. Sequential mode\n"
            "3. Random mode\n"
            "4. How to play\n"
            "5. Exit\n");

        switch (mode) {
            case 1: knightsStandardMode(); break;
            case 2: knightsSequentialMode(); break;
            case 3: knightsRandomMode(); break;
            case 4:
                std::cout << "\nHow to play: Place the knight on a square, then keep moving it "
                             "in L-shapes so that it visits every square exactly once.\n";
                break;
            case 5: return;
            default: std::cout << "Invalid choice. Please try again.\n";
        }
    }
}
