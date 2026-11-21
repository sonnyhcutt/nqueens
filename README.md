# Puzzles and Problems

A terminal game in C++ with two classic chess puzzles:

- **N-Queens**: place *n* queens on an *n × n* board so that no two queens attack each other.
- **Knight's Tour**: move a knight around the board so it lands on every square exactly once.

Each puzzle has three modes:

| Mode       | N-Queens                                         | Knight's Tour                                       |
|------------|--------------------------------------------------|-----------------------------------------------------|
| Standard   | You pick the board size (1–12); you can save and load boards | You pick the board size (5 or larger) and the starting square |
| Sequential | Board sizes go up from 1×1 to 8×8                | Starts at 5×5 and gets one size bigger after each completed tour |
| Random     | The board size is random (4–12)                  | 5×5 board with a random starting square             |

If your N-Queens answer is wrong, the game finds a correct one by backtracking and shows it to you.

## Building

You need a C++17 compiler (GCC, Clang, or MSVC).

### With CMake

```sh
cmake -S . -B build
cmake --build build
./build/puzzles
```

Run the tests:

```sh
ctest --test-dir build --output-on-failure
```

### With Make

```sh
make          # builds build/puzzles
make run      # builds and runs the game
make test     # builds and runs the tests
make clean
```

## How to play

- **N-Queens**: name squares by column letter and row number, such as `a1` or `c4`.
- **Knight's Tour**: type the row and then the column as numbers starting at 0, such as `2 1`. Type `-1 -1` to quit the current tour.

### Saved board format

Standard N-Queens mode can save boards to text files and load them back. The first line is the board size. Each line after that is one row, using `.` for an empty square and `Q` for a queen:

```
4
.Q..
...Q
Q...
..Q.
```

## N-Queens solvers and benchmark

`include/nqueens_solvers.h` has two solvers that work on any board size. Both check attacks in O(1) time using counters for each column and diagonal.

- **Backtracking** (`solveBacktracking`): a depth-first search that tries columns left to right in each row. It always gives an answer: it either finds a solution or proves there isn't one. Its runtime varies wildly from one *n* to the next.
- **Min-conflicts** (`solveMinConflicts`): a local search based on Minton et al. (1992). It places queens greedily at first, then keeps moving a random attacked queen to the column in its row with the fewest attacks. If it doesn't finish within `max(100, 4n)` moves, it starts over. It can't prove that no solution exists, so it gives up right away for *n* = 2 and 3.

Run the benchmark:

```sh
make bench                                            # n = 4..100, CSV in build/
./build/nqueens_bench --max-n 60 --trials 30 --timeout-ms 5000 --csv out.csv
```

### Results (n = 4–100)

These numbers are from an Apple M2, built with `-O2`, running 15 trials per *n*. Each backtracking run was stopped after 2 seconds. The full data is in [`benchmarks/nqueens_4_100.csv`](benchmarks/nqueens_4_100.csv).

| n   | Backtracking time | Backtracking nodes | Min-conflicts median | Min-conflicts worst | Min-conflicts moves |
|-----|------------------:|-------------------:|---------------------:|--------------------:|--------------------:|
| 8   | 5.5 µs            | 113                | 10 µs                | 42 µs               | 58                  |
| 16  | 0.76 ms           | 10,052             | 21 µs                | 76 µs               | 83                  |
| 20  | 6.2 ms            | 199,635            | 9 µs                 | 48 µs               | 69                  |
| 22  | 56 ms             | 1,737,188          | 7 µs                 | 23 µs               | 50                  |
| 28  | 116 ms            | 3,006,298          | 15 µs                | 38 µs               | 76                  |
| 31  | 549 ms            | 13,186,250         | 12 µs                | 22 µs               | 46                  |
| 32–100 | **timed out (> 2 s)** | > 18–47 million | 12–53 µs        | 22–108 µs           | 32–83               |

What this shows:

- **Backtracking grows exponentially, and unevenly.** Its time goes up by roughly 10× every few sizes, and nearby sizes can differ a lot (n = 21 takes 0.26 ms, while n = 22 takes 56 ms). From n = 32 up, no run finished within 2 s.
- **Min-conflicts grows about as fast as n², and in practice almost linearly.** It needed about 50–70 repair moves no matter what *n* was. Each move scans O(n) columns, and the greedy starting board costs O(n²). Going from n = 10 to n = 100 only took the median time from 14 µs to 53 µs. It solved all 1,455 runs.
- **Backtracking wins at very small n (up to about 13, except 12).** At those sizes the search is only a few dozen nodes. Min-conflicts sometimes gets stuck and has to restart, especially at n = 6, which has only 4 solutions.

## Project layout

```
.
├── include/          Public headers
│   ├── game.h          Game base class and the two game classes
│   ├── nqueens.h       N-Queens board checks, display, and game modes
│   ├── nqueens_solvers.h  Backtracking and min-conflicts solvers
│   ├── knights_tour.h  Knight's Tour logic, solver, and modes
│   ├── board_io.h      Saving and loading boards
│   └── input.h         Safe console input helpers
├── src/              Source files that match the headers, plus main.cpp
├── bench/            Solver benchmark (nqueens_bench)
├── benchmarks/       Saved benchmark results (CSV)
├── tests/            Unit tests with no outside dependencies
├── CMakeLists.txt
└── Makefile
```

## License

[MIT](LICENSE)
