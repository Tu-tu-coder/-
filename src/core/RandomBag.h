#ifndef RANDOMBAG_H
#define RANDOMBAG_H

#include "GameConfig.h"
#include "Tetromino.h"
#include <vector>
#include <random>

// 7-bag randomizer: deals all 7 pieces in random order before refilling
// Guarantees fair distribution (no long droughts of a piece type)
class RandomBag {
public:
    RandomBag();

    // Get next piece (may refill bag if empty)
    Tetromino nextPiece();

    // Peek at next piece without consuming it
    Tetromino peekNext() const;

    // Peek N pieces ahead (0 = next)
    Tetromino peekAhead(int n) const;

    // Reset bag state
    void reset();

    // Set seed for reproducible games
    void setSeed(unsigned int seed);

private:
    void refill();

    std::mt19937 m_rng;
    std::vector<GameConfig::PieceType> m_bag;
    std::vector<GameConfig::PieceType> m_queue; // upcoming pieces (always >= 1)

    static constexpr int kPreviewQueue = 7; // keep at least this many queued
};

#endif // RANDOMBAG_H
