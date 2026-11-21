#include "nqueens_solvers.h"

#include <algorithm>
#include <limits>

namespace {

using Clock = std::chrono::steady_clock;

// Counts how many queens sit on each column and each diagonal.
// Diagonals: row - col + n - 1 (top-left to bottom-right), row + col (anti).
class AttackCounters {
public:
    explicit AttackCounters(int n) : n_(n), cols_(n, 0), diags_(2 * n - 1, 0), antiDiags_(2 * n - 1, 0) {}

    void add(int row, int col, int delta) {
        cols_[col] += delta;
        diags_[row - col + n_ - 1] += delta;
        antiDiags_[row + col] += delta;
    }

    // Queens attacking square (row, col), not counting a queen on that square itself.
    int attacks(int row, int col, bool occupied) const {
        const int total = cols_[col] + diags_[row - col + n_ - 1] + antiDiags_[row + col];
        return occupied ? total - 3 : total;
    }

    void clear() {
        std::fill(cols_.begin(), cols_.end(), 0);
        std::fill(diags_.begin(), diags_.end(), 0);
        std::fill(antiDiags_.begin(), antiDiags_.end(), 0);
    }

private:
    int n_;
    std::vector<int> cols_;
    std::vector<int> diags_;
    std::vector<int> antiDiags_;
};

class Deadline {
public:
    explicit Deadline(std::chrono::nanoseconds limit)
        : enabled_(limit > std::chrono::nanoseconds::zero()), end_(Clock::now() + limit) {}

    bool expired() const { return enabled_ && Clock::now() >= end_; }

private:
    bool enabled_;
    Clock::time_point end_;
};

class Backtracker {
public:
    Backtracker(int n, std::chrono::nanoseconds limit)
        : n_(n), counters_(n), queens_(n, -1), deadline_(limit) {}

    SolveResult run() {
        SolveResult result;
        result.solved = place(0);
        result.timedOut = timedOut_;
        result.steps = nodes_;
        if (result.solved) result.queens = queens_;
        return result;
    }

private:
    bool place(int row) {
        if (row == n_) return true;
        for (int col = 0; col < n_; ++col) {
            // Reading the clock is slow, so only check it every 4096 candidates.
            if ((++checks_ & 0xFFF) == 0 && deadline_.expired()) {
                timedOut_ = true;
                return false;
            }
            if (counters_.attacks(row, col, false) != 0) continue;

            ++nodes_;
            counters_.add(row, col, +1);
            queens_[row] = col;
            if (place(row + 1)) return true;
            if (timedOut_) return false;
            counters_.add(row, col, -1);
        }
        return false;
    }

    int n_;
    AttackCounters counters_;
    Placement queens_;
    Deadline deadline_;
    std::uint64_t nodes_ = 0;
    std::uint64_t checks_ = 0;
    bool timedOut_ = false;
};

// Puts each row's queen in the column with the fewest attacks from the
// queens already placed, picking at random among ties. This usually leaves
// only a few conflicts to repair.
void greedyInitialize(int n, Placement& queens, AttackCounters& counters, std::mt19937& rng,
                      std::vector<int>& candidates) {
    counters.clear();
    for (int row = 0; row < n; ++row) {
        int best = std::numeric_limits<int>::max();
        candidates.clear();
        for (int col = 0; col < n; ++col) {
            const int a = counters.attacks(row, col, false);
            if (a < best) {
                best = a;
                candidates.clear();
            }
            if (a == best) candidates.push_back(col);
        }
        std::uniform_int_distribution<std::size_t> pick(0, candidates.size() - 1);
        queens[row] = candidates[pick(rng)];
        counters.add(row, queens[row], +1);
    }
}

} // namespace

bool isValidPlacement(const Placement& queens) {
    const int n = static_cast<int>(queens.size());
    std::vector<bool> cols(n, false);
    std::vector<bool> diags(2 * n, false);
    std::vector<bool> antiDiags(2 * n, false);
    for (int row = 0; row < n; ++row) {
        const int col = queens[row];
        if (col < 0 || col >= n) return false;
        const int d = row - col + n;
        const int a = row + col;
        if (cols[col] || diags[d] || antiDiags[a]) return false;
        cols[col] = diags[d] = antiDiags[a] = true;
    }
    return true;
}

SolveResult solveBacktracking(int n, std::chrono::nanoseconds timeLimit) {
    if (n <= 0) return {};
    return Backtracker(n, timeLimit).run();
}

SolveResult solveMinConflicts(int n, std::mt19937& rng, const MinConflictsOptions& options) {
    SolveResult result;
    if (n <= 0 || n == 2 || n == 3) return result; // no solution exists

    const std::uint64_t maxSteps =
        options.maxStepsPerRestart != 0 ? options.maxStepsPerRestart
                                        : std::max<std::uint64_t>(100, 4 * static_cast<std::uint64_t>(n));
    const Deadline deadline(options.timeLimit);

    Placement queens(n);
    AttackCounters counters(n);
    std::vector<int> conflicted;
    std::vector<int> candidates;
    conflicted.reserve(n);
    candidates.reserve(n);

    while (true) {
        greedyInitialize(n, queens, counters, rng, candidates);

        for (std::uint64_t step = 0; step < maxSteps; ++step) {
            conflicted.clear();
            for (int row = 0; row < n; ++row) {
                if (counters.attacks(row, queens[row], true) > 0) conflicted.push_back(row);
            }
            if (conflicted.empty()) {
                result.solved = true;
                result.queens = queens;
                return result;
            }
            if (deadline.expired()) {
                result.timedOut = true;
                return result;
            }

            std::uniform_int_distribution<std::size_t> pickRow(0, conflicted.size() - 1);
            const int row = conflicted[pickRow(rng)];

            // Move this row's queen to the column where it is attacked least.
            counters.add(row, queens[row], -1);
            int best = std::numeric_limits<int>::max();
            candidates.clear();
            for (int col = 0; col < n; ++col) {
                const int a = counters.attacks(row, col, false);
                if (a < best) {
                    best = a;
                    candidates.clear();
                }
                if (a == best) candidates.push_back(col);
            }
            std::uniform_int_distribution<std::size_t> pickCol(0, candidates.size() - 1);
            queens[row] = candidates[pickCol(rng)];
            counters.add(row, queens[row], +1);
            ++result.steps;
        }

        ++result.restarts;
    }
}
