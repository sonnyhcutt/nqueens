#ifndef PUZZLES_INPUT_H
#define PUZZLES_INPUT_H

#include <string>

// Console input helpers. Each prints the prompt, then re-prompts on malformed
// input instead of leaving std::cin in a failed state. If input ends (EOF),
// the program exits cleanly.

int readInt(const std::string& prompt);
char readChar(const std::string& prompt);
std::string readWord(const std::string& prompt);

// Returns true if the user answers 'y' or 'Y'.
bool askYesNo(const std::string& prompt);

#endif // PUZZLES_INPUT_H
