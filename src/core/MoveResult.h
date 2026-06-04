#ifndef MOVERESULT_H
#define MOVERESULT_H

enum class MoveResult {
    OK,           // Move succeeded
    OutOfBounds,  // Move outside board
    Collision,    // Move blocked by existing blocks
    Locked,       // Piece has landed and locked
    GameOver      // Locked piece is above visible area
};

#endif // MOVERESULT_H
