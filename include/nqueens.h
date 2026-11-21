#ifndef PUZZLES_NQUEENS_H
#define PUZZLES_NQUEENS_H

#include <vector>

// Player board: '.' for empty, 'Q' for a queen.
using CharBoard = std::vector<std::vector<char>>;
// Solver board: 1 for a queen, 0 for empty.
using IntBoard = std::vector<std::vector<int>>;

constexpr int kMaxQueensBoardSize = 12;

// ---- Core logic (no I/O) ----

// True if the board holds exactly n queens and no two attack each other.
bool checkSolution(const CharBoard& board);

// Fills an empty n x n `board` with a solution found by backtracking
// (see nqueens_solvers.h). Returns false if none exists (n = 2 or n = 3).
bool solveNQueens(IntBoard& board);

// ---- Console display / interaction ----

void printPlayerBoard(const CharBoard& board);
void printSolutionBoard(const IntBoard& board);
void placeQueensManually(CharBoard& board);

void queensStandardMode();
void queensSequentialMode();
void queensRandomMode();

#endif // PUZZLES_NQUEENS_H
