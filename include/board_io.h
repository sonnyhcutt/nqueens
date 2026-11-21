#ifndef PUZZLES_BOARD_IO_H
#define PUZZLES_BOARD_IO_H

#include <string>

#include "nqueens.h"

// File format: first line is the board size n, followed by n lines of n
// characters each ('.' or 'Q').

// Returns true on success.
bool saveBoard(const CharBoard& board, const std::string& filename);

// Returns an empty board if the file can't be read or is malformed.
CharBoard loadBoard(const std::string& filename);

#endif // PUZZLES_BOARD_IO_H
