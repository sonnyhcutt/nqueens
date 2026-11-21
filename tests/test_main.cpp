// Minimal dependency-free test runner for the puzzle logic.

#include <cstdio>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "board_io.h"
#include "knights_tour.h"
#include "nqueens.h"
#include "nqueens_solvers.h"

namespace {

int failures = 0;
int checks = 0;

#define CHECK(cond)                                                              \
    do {                                                                         \
        ++checks;                                                                \
        if (!(cond)) {                                                           \
            ++failures;                                                          \
            std::cerr << __FILE__ << ":" << __LINE__ << ": CHECK failed: " #cond \
                      << '\n';                                                   \
        }                                                                        \
    } while (0)

CharBoard toCharBoard(const IntBoard& board) {
    CharBoard result;
    for (const auto& row : board) {
        std::vector<char> out;
        for (int cell : row) out.push_back(cell == 1 ? 'Q' : '.');
        result.push_back(out);
    }
    return result;
}

void testCheckSolution() {
    CHECK(checkSolution({{'Q'}}));
    CHECK(checkSolution({
        {'.', 'Q', '.', '.'},
        {'.', '.', '.', 'Q'},
        {'Q', '.', '.', '.'},
        {'.', '.', 'Q', '.'},
    }));
    // Same row.
    CHECK(!checkSolution({{'Q', 'Q'}, {'.', '.'}}));
    // Diagonal.
    CHECK(!checkSolution({
        {'Q', '.', '.', '.'},
        {'.', 'Q', '.', '.'},
        {'.', '.', '.', 'Q'},
        {'.', '.', 'Q', '.'},
    }));
    // Too few queens.
    CHECK(!checkSolution({{'.', '.'}, {'.', '.'}}));
}

void testSolveNQueens() {
    for (int n = 1; n <= kMaxQueensBoardSize; ++n) {
        IntBoard board(n, std::vector<int>(n, 0));
        const bool solved = solveNQueens(board);
        if (n == 2 || n == 3) {
            CHECK(!solved);
        } else {
            CHECK(solved);
            CHECK(checkSolution(toCharBoard(board)));
        }
    }
}

void testIsValidPlacement() {
    CHECK(isValidPlacement({0}));
    CHECK(isValidPlacement({1, 3, 0, 2}));
    CHECK(!isValidPlacement({0, 1, 2, 3}));  // diagonal
    CHECK(!isValidPlacement({1, 1, 0, 2}));  // same column
    CHECK(!isValidPlacement({1, 3, 0, 4}));  // off the board
}

void testSolveBacktracking() {
    CHECK(!solveBacktracking(2).solved);
    CHECK(!solveBacktracking(3).solved);
    CHECK(!solveBacktracking(3).timedOut);
    for (int n : {1, 4, 8, 20, 30}) {
        const SolveResult r = solveBacktracking(n);
        CHECK(r.solved);
        CHECK(isValidPlacement(r.queens));
    }
}

void testSolveMinConflicts() {
    std::mt19937 rng(12345);
    CHECK(!solveMinConflicts(2, rng).solved);
    CHECK(!solveMinConflicts(3, rng).solved);
    for (int n : {1, 4, 5, 6, 7, 8, 10, 25, 50, 100, 500, 2000}) {
        for (int trial = 0; trial < 3; ++trial) {
            const SolveResult r = solveMinConflicts(n, rng);
            CHECK(r.solved);
            CHECK(isValidPlacement(r.queens));
        }
    }
}

void testKnightMoves() {
    CHECK(isKnightMove(0, 0, 1, 2));
    CHECK(isKnightMove(0, 0, 2, 1));
    CHECK(isKnightMove(3, 3, 1, 2));
    CHECK(!isKnightMove(0, 0, 1, 1));
    CHECK(!isKnightMove(0, 0, 0, 0));

    KnightBoard board(5, std::vector<int>(5, 0));
    CHECK(isValidKnightMove(0, 0, board));
    CHECK(!isValidKnightMove(-1, 0, board));
    CHECK(!isValidKnightMove(0, 5, board));
    board[2][2] = 3;
    CHECK(!isValidKnightMove(2, 2, board));
}

void testSolveKnightsTour() {
    const int n = 5;
    KnightBoard board(n, std::vector<int>(n, 0));
    CHECK(solveKnightsTour(board, 0, 0));

    // Every square visited exactly once, and consecutive steps are knight moves.
    std::vector<std::pair<int, int>> order(n * n + 1, {-1, -1});
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            const int step = board[r][c];
            CHECK(step >= 1 && step <= n * n);
            if (step >= 1 && step <= n * n) order[step] = {r, c};
        }
    }
    CHECK(order[1] == std::make_pair(0, 0));
    for (int step = 2; step <= n * n; ++step) {
        CHECK(isKnightMove(order[step - 1].first, order[step - 1].second, order[step].first,
                           order[step].second));
    }
}

void testBoardRoundTrip() {
    const std::string path = "puzzles_test_board.txt";
    const CharBoard board = {
        {'.', 'Q', '.', '.'},
        {'.', '.', '.', 'Q'},
        {'Q', '.', '.', '.'},
        {'.', '.', 'Q', '.'},
    };
    CHECK(saveBoard(board, path));
    CHECK(loadBoard(path) == board);
    std::remove(path.c_str());

    CHECK(loadBoard("does_not_exist.txt").empty());
}

} // namespace

int main() {
    testCheckSolution();
    testSolveNQueens();
    testIsValidPlacement();
    testSolveBacktracking();
    testSolveMinConflicts();
    testKnightMoves();
    testSolveKnightsTour();
    testBoardRoundTrip();

    std::cout << (checks - failures) << "/" << checks << " checks passed\n";
    return failures == 0 ? 0 : 1;
}
