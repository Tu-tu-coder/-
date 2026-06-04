#ifndef TETRISBOARD_H
#define TETRISBOARD_H

#include "GameConfig.h"
#include "Tetromino.h"
#include "MoveResult.h"
#include <vector>
#include <utility>

class TetrisBoard {
public:
    TetrisBoard();

    // Reset board to empty state
    void reset();

    // Access grid cell. Returns GameConfig::kEmptyCell (-1) if empty,
    // or the PieceType int value (0-6) if occupied.
    int cellAt(int row, int col) const;
    int cellAtVisible(int visibleRow, int col) const; // row 0 = top of visible area

    int width() const { return GameConfig::kBoardWidth; }
    int height() const { return GameConfig::kBoardHeight; }
    int totalRows() const { return GameConfig::kTotalRows; }

    // Test if a piece at (boardRow, boardCol) with given rotation can be placed
    bool canPlace(const Tetromino& piece, int boardRow, int boardCol) const;

    // Lock a piece into the board, returns which rows were cleared
    std::vector<int> lockPiece(const Tetromino& piece, int boardRow, int boardCol);

    // Clear full rows and shift everything down. Returns indices of cleared rows.
    std::vector<int> clearFullRows();

    // Get ghost piece Y position (hard drop target row)
    int ghostRow(const Tetromino& piece, int boardCol) const;

    // Clear a specific row (for skills like Earthquake)
    void clearRow(int row);

    // Apply gravity to floating blocks above cleared rows
    void applyGravity();

    // Check if game is over (blocks in buffer zone)
    bool isGameOver() const;

    // For debugging: print board to string
    QString toString() const;

private:
    // Grid: totalRows x width, stored as [row][col]
    // Row 0-1 = buffer zone (hidden), Row 2-21 = visible area
    int m_grid[GameConfig::kTotalRows][GameConfig::kBoardWidth];

    bool isRowFull(int row) const;
    void clearRowInternal(int row);
    bool isInBounds(int row, int col) const;
};

#endif // TETRISBOARD_H
