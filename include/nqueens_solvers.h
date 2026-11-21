#ifndef PUZZLES_NQUEENS_SOLVERS_H
#define PUZZLES_NQUEENS_SOLVERS_H

#include <chrono>
#include <cstdint>
#include <random>
#include <vector>

// N-Queens solvers that work on any board size. Both use O(1) attack checks
// (per-column and per-diagonal counters), so a benchmark compares the
// algorithms themselves rather than how the board is stored.

// queens[row] = column of the queen in that row (one queen per row).
using Placement = std::vector<int>;

struct SolveResult {
    bool solved = false;
    bool timedOut = false;
    Placement queens;
    // Backtracking: number of queens placed (search nodes).
    // Min-conflicts: number of repair moves, counted across all restarts.
    std::uint64_t steps = 0;
    // Min-conflicts only: how many times it gave up on a board and started over.
    std::uint64_t restarts = 0;
};

// True if `queens` describes n non-attacking queens on an n x n board.
bool isValidPlacement(const Placement& queens);

// Depth-first backtracking that tries columns left to right in each row. It is
// complete: it either finds a solution or proves there is none (n = 2, 3).
// Its runtime is very uneven, and some sizes take a very long time. A
// non-zero `timeLimit` stops the search and sets `timedOut`.
SolveResult solveBacktracking(int n,
                              std::chrono::nanoseconds timeLimit = std::chrono::nanoseconds::zero());

struct MinConflictsOptions {
    // Repair moves allowed before a random restart. 0 = automatic (max(100, 4n)).
    std::uint64_t maxStepsPerRestart = 0;
    // 0 = no time limit.
    std::chrono::nanoseconds timeLimit = std::chrono::nanoseconds::zero();
};

// Min-conflicts local search (Minton et al., 1992). It starts from a greedy
// board, then keeps picking a random attacked queen and moving it within its
// row to the column with the fewest attacks (ties broken at random). It is
// incomplete: it can't prove there is no solution, so it returns unsolved
// right away for n = 2 and 3.
SolveResult solveMinConflicts(int n, std::mt19937& rng, const MinConflictsOptions& options = {});

#endif // PUZZLES_NQUEENS_SOLVERS_H
