#ifndef PUZZLES_GAME_H
#define PUZZLES_GAME_H

// Base class for all games.
class Game {
public:
    virtual ~Game() = default;
    virtual void play() = 0;
};

// N-Queens puzzle: place n queens on an n x n board so none attack each other.
class NQueensGame : public Game {
public:
    void play() override;
};

// Knight's Tour puzzle: move a knight so it visits every square exactly once.
class KnightsGame : public Game {
public:
    void play() override;
};

#endif // PUZZLES_GAME_H
