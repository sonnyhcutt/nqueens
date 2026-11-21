#ifndef PUZZLES_KNIGHTS_TOUR_H
#define PUZZLES_KNIGHTS_TOUR_H

#include <vector>

// Board cell values: 0 = unvisited, >0 = visited (move number), -1 = knight.
using KnightBoard = std::vector<std::vector<int>>;

constexpr int kMinKnightsBoardSize = 5;

// ---- Core logic (no I/O) ----

// True if (row, col) is on the board and has not been visited.
bool isValidKnightMove(int row, int col, const KnightBoard& board);

// True if moving from (fromRow, fromCol) to (toRow, toCol) is an L-shaped knight move.
bool isKnightMove(int fromRow, int fromCol, int toRow, int toCol);

// Finds a full tour from (startRow, startCol) using backtracking. On success,
// each cell holds its visit order (1..n*n). Returns false if none exists.
bool solveKnightsTour(KnightBoard& board, int startRow, int startCol);

// ---- Console display / interaction ----

void displayKnightsBoard(const KnightBoard& board);

// Runs an interactive tour. Returns true if the player completes it,
// false if they quit.
bool playKnightsTour(int boardSize, int startRow, int startCol);

void knightsStandardMode();
void knightsSequentialMode();
void knightsRandomMode();

#endif // PUZZLES_KNIGHTS_TOUR_H
