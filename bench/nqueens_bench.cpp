// Benchmarks backtracking against min-conflicts for N-Queens over a range of n.
//
// Usage: nqueens_bench [--min-n 4] [--max-n 100] [--trials 15]
//                      [--timeout-ms 2000] [--seed 1] [--csv results.csv]
//
// Backtracking always gives the same result for a given n, so each of its
// trials times the same search; if the first run hits the timeout, it isn't
// repeated. Min-conflicts is randomized, so each trial uses a different seed.
// Every solution is checked before it is counted.

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "nqueens_solvers.h"

namespace {

using Clock = std::chrono::steady_clock;

struct Options {
    int minN = 4;
    int maxN = 100;
    int trials = 15;
    long timeoutMs = 2000;
    std::uint32_t seed = 1;
    std::string csvPath;
};

[[noreturn]] void usage(const char* prog) {
    std::cerr << "Usage: " << prog
              << " [--min-n N] [--max-n N] [--trials K] [--timeout-ms MS] [--seed S] [--csv FILE]\n";
    std::exit(2);
}

Options parseArgs(int argc, char** argv) {
    Options o;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (i + 1 >= argc) usage(argv[0]);
        const std::string value = argv[++i];
        try {
            if (arg == "--min-n") o.minN = std::stoi(value);
            else if (arg == "--max-n") o.maxN = std::stoi(value);
            else if (arg == "--trials") o.trials = std::stoi(value);
            else if (arg == "--timeout-ms") o.timeoutMs = std::stol(value);
            else if (arg == "--seed") o.seed = static_cast<std::uint32_t>(std::stoul(value));
            else if (arg == "--csv") o.csvPath = value;
            else usage(argv[0]);
        } catch (const std::exception&) {
            usage(argv[0]);
        }
    }
    if (o.minN < 1 || o.maxN < o.minN || o.trials < 1 || o.timeoutMs < 1) usage(argv[0]);
    return o;
}

double median(std::vector<double> v) {
    std::sort(v.begin(), v.end());
    const std::size_t m = v.size() / 2;
    return v.size() % 2 ? v[m] : (v[m - 1] + v[m]) / 2.0;
}

template <typename F>
double timeMicros(F&& f) {
    const auto start = Clock::now();
    f();
    return std::chrono::duration<double, std::micro>(Clock::now() - start).count();
}

void requireValid(const SolveResult& r, const char* solver, int n) {
    if (r.solved && !isValidPlacement(r.queens)) {
        std::cerr << "BUG: " << solver << " returned an invalid placement for n = " << n << '\n';
        std::exit(1);
    }
}

struct Row {
    int n;
    std::string btStatus; // solved / none / timeout
    double btMedianUs;
    std::uint64_t btNodes;
    double mcMedianUs;
    double mcMaxUs;
    double mcMeanSteps;
    double mcMeanRestarts;
    int mcSolved;
};

} // namespace

int main(int argc, char** argv) {
    const Options opt = parseArgs(argc, argv);
    const auto timeout = std::chrono::milliseconds(opt.timeoutMs);

    std::cout << "N-Queens: backtracking vs min-conflicts (" << opt.trials
              << " trials per n, backtracking timeout " << opt.timeoutMs << " ms)\n\n"
              << std::setw(4) << "n" << " | " << std::setw(12) << "BT median us" << std::setw(14)
              << "BT nodes" << " | " << std::setw(12) << "MC median us" << std::setw(12) << "MC max us"
              << std::setw(10) << "MC steps" << std::setw(10) << "restarts" << std::setw(8) << "solved"
              << '\n'
              << std::string(100, '-') << '\n';

    std::vector<Row> rows;
    for (int n = opt.minN; n <= opt.maxN; ++n) {
        Row row{n, "", 0, 0, 0, 0, 0, 0, 0};

        // --- Backtracking ---
        std::vector<double> btTimes;
        for (int t = 0; t < opt.trials; ++t) {
            SolveResult r;
            btTimes.push_back(timeMicros([&] { r = solveBacktracking(n, timeout); }));
            requireValid(r, "backtracking", n);
            row.btNodes = r.steps;
            row.btStatus = r.solved ? "solved" : (r.timedOut ? "timeout" : "none");
            if (r.timedOut) break;
        }
        row.btMedianUs = median(btTimes);

        // --- Min-conflicts ---
        std::vector<double> mcTimes;
        std::uint64_t totalSteps = 0;
        std::uint64_t totalRestarts = 0;
        for (int t = 0; t < opt.trials; ++t) {
            std::mt19937 rng(opt.seed * 1000003u + static_cast<std::uint32_t>(n) * 7919u +
                             static_cast<std::uint32_t>(t));
            MinConflictsOptions mcOpts;
            mcOpts.timeLimit = timeout;
            SolveResult r;
            mcTimes.push_back(timeMicros([&] { r = solveMinConflicts(n, rng, mcOpts); }));
            requireValid(r, "min-conflicts", n);
            totalSteps += r.steps;
            totalRestarts += r.restarts;
            row.mcSolved += r.solved ? 1 : 0;
        }
        row.mcMedianUs = median(mcTimes);
        row.mcMaxUs = *std::max_element(mcTimes.begin(), mcTimes.end());
        row.mcMeanSteps = static_cast<double>(totalSteps) / opt.trials;
        row.mcMeanRestarts = static_cast<double>(totalRestarts) / opt.trials;

        std::ostringstream bt;
        if (row.btStatus == "timeout") {
            bt << ">" << opt.timeoutMs * 1000;
        } else {
            bt << std::fixed << std::setprecision(1) << row.btMedianUs;
        }
        std::cout << std::setw(4) << n << " | " << std::setw(12) << bt.str() << std::setw(14)
                  << (row.btStatus == "timeout" ? ">" + std::to_string(row.btNodes)
                                                : std::to_string(row.btNodes))
                  << " | " << std::fixed << std::setprecision(1) << std::setw(12) << row.mcMedianUs
                  << std::setw(12) << row.mcMaxUs << std::setw(10) << row.mcMeanSteps << std::setw(10)
                  << row.mcMeanRestarts << std::setw(5) << row.mcSolved << "/" << opt.trials << '\n';
        rows.push_back(row);
    }

    if (!opt.csvPath.empty()) {
        std::ofstream csv(opt.csvPath);
        if (!csv) {
            std::cerr << "Could not write " << opt.csvPath << '\n';
            return 1;
        }
        csv << "n,bt_status,bt_median_us,bt_nodes,mc_median_us,mc_max_us,mc_mean_steps,"
               "mc_mean_restarts,mc_solved,trials\n";
        for (const Row& r : rows) {
            csv << r.n << ',' << r.btStatus << ',' << r.btMedianUs << ',' << r.btNodes << ','
                << r.mcMedianUs << ',' << r.mcMaxUs << ',' << r.mcMeanSteps << ',' << r.mcMeanRestarts
                << ',' << r.mcSolved << ',' << opt.trials << '\n';
        }
        std::cout << "\nWrote " << opt.csvPath << '\n';
    }
    return 0;
}
