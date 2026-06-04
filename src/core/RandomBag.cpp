#include "RandomBag.h"
#include <algorithm>
#include <chrono>

RandomBag::RandomBag()
{
    auto seed = static_cast<unsigned int>(
        std::chrono::steady_clock::now().time_since_epoch().count());
    m_rng.seed(seed);
    reset();
}

void RandomBag::reset()
{
    m_bag.clear();
    m_queue.clear();
    // Fill initial queue
    for (int i = 0; i < kPreviewQueue; ++i) {
        if (m_bag.empty()) refill();
        m_queue.push_back(m_bag.back());
        m_bag.pop_back();
    }
}

void RandomBag::setSeed(unsigned int seed)
{
    m_rng.seed(seed);
    reset();
}

void RandomBag::refill()
{
    m_bag = {
        GameConfig::PieceType::I,
        GameConfig::PieceType::O,
        GameConfig::PieceType::T,
        GameConfig::PieceType::S,
        GameConfig::PieceType::Z,
        GameConfig::PieceType::J,
        GameConfig::PieceType::L
    };
    std::shuffle(m_bag.begin(), m_bag.end(), m_rng);
}

Tetromino RandomBag::nextPiece()
{
    Tetromino piece(m_queue.front());
    m_queue.erase(m_queue.begin());

    // Keep queue topped up
    while (static_cast<int>(m_queue.size()) < kPreviewQueue) {
        if (m_bag.empty()) refill();
        m_queue.push_back(m_bag.back());
        m_bag.pop_back();
    }

    return piece;
}

Tetromino RandomBag::peekNext() const
{
    return peekAhead(0);
}

Tetromino RandomBag::peekAhead(int n) const
{
    if (n >= 0 && n < static_cast<int>(m_queue.size())) {
        return Tetromino(m_queue[n]);
    }

    // Fallback: reconstruct from bag (shouldn't normally happen)
    if (m_queue.empty()) return Tetromino(GameConfig::PieceType::I);
    return Tetromino(m_queue[0]);
}
