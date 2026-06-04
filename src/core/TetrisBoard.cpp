#include "TetrisBoard.h"
#include <algorithm>
#include <QString>

TetrisBoard::TetrisBoard()
{
    reset();
}

void TetrisBoard::reset()
{
    for (int r = 0; r < GameConfig::kTotalRows; ++r) {
        for (int c = 0; c < GameConfig::kBoardWidth; ++c) {
            m_grid[r][c] = GameConfig::kEmptyCell;
        }
    }
}

int TetrisBoard::cellAt(int row, int col) const
{
    if (!isInBounds(row, col)) return GameConfig::kEmptyCell;
    return m_grid[row][col];
}

int TetrisBoard::cellAtVisible(int visibleRow, int col) const
{
    return cellAt(visibleRow + GameConfig::kBufferRows, col);
}

bool TetrisBoard::isInBounds(int row, int col) const
{
    return row >= 0 && row < GameConfig::kTotalRows
        && col >= 0 && col < GameConfig::kBoardWidth;
}

bool TetrisBoard::canPlace(const Tetromino& piece, int boardRow, int boardCol) const
{
    auto cells = piece.occupiedCells();
    for (auto& [r, c] : cells) {
        int absR = boardRow + r;
        int absC = boardCol + c;

        // Allow above the board (buffer zone) but not below or sideways
        if (absC < 0 || absC >= GameConfig::kBoardWidth) return false;
        if (absR >= GameConfig::kTotalRows) return false;
        if (absR < 0) continue; // above buffer is OK

        if (m_grid[absR][absC] != GameConfig::kEmptyCell) return false;
    }
    return true;
}

bool TetrisBoard::canPlaceShape(const Tetromino::Shape& shape, int boardRow, int boardCol) const
{
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (!shape[r][c]) continue;
            int absR = boardRow + r;
            int absC = boardCol + c;
            if (absC < 0 || absC >= GameConfig::kBoardWidth) return false;
            if (absR >= GameConfig::kTotalRows) return false;
            if (absR < 0) continue;
            if (m_grid[absR][absC] != GameConfig::kEmptyCell) return false;
        }
    }
    return true;
}

std::vector<int> TetrisBoard::lockPiece(const Tetromino& piece, int boardRow, int boardCol)
{
    int typeIdx = static_cast<int>(piece.type());
    auto cells = piece.occupiedCells();

    for (auto& [r, c] : cells) {
        int absR = boardRow + r;
        int absC = boardCol + c;
        if (absR >= 0 && absR < GameConfig::kTotalRows
            && absC >= 0 && absC < GameConfig::kBoardWidth) {
            m_grid[absR][absC] = typeIdx;
        }
    }

    return clearFullRows();
}

std::vector<int> TetrisBoard::clearFullRows()
{
    std::vector<int> allCleared;

    // Loop until no more full rows (handles chain clears)
    while (true) {
        // Find all currently full rows
        std::vector<int> full;
        for (int r = 0; r < GameConfig::kTotalRows; ++r) {
            if (isRowFull(r)) {
                full.push_back(r);
            }
        }
        if (full.empty()) break;

        // Clear from bottom to top
        for (int i = static_cast<int>(full.size()) - 1; i >= 0; --i) {
            clearRowInternal(full[i]);
            allCleared.push_back(full[i]);
        }
    }

    return allCleared;
}

bool TetrisBoard::isRowFull(int row) const
{
    for (int c = 0; c < GameConfig::kBoardWidth; ++c) {
        if (m_grid[row][c] == GameConfig::kEmptyCell)
            return false;
    }
    return true;
}

void TetrisBoard::clearRow(int row)
{
    if (row >= 0 && row < GameConfig::kTotalRows) {
        clearRowInternal(row);
    }
}

void TetrisBoard::clearRowInternal(int row)
{
    // Shift all rows above down by 1
    for (int r = row; r > 0; --r) {
        for (int c = 0; c < GameConfig::kBoardWidth; ++c) {
            m_grid[r][c] = m_grid[r - 1][c];
        }
    }
    // Top row becomes empty
    for (int c = 0; c < GameConfig::kBoardWidth; ++c) {
        m_grid[0][c] = GameConfig::kEmptyCell;
    }
}

void TetrisBoard::applyGravity()
{
    // After clearing rows with gaps, compact blocks downward
    for (int c = 0; c < GameConfig::kBoardWidth; ++c) {
        int writeRow = GameConfig::kTotalRows - 1;
        for (int r = GameConfig::kTotalRows - 1; r >= 0; --r) {
            if (m_grid[r][c] != GameConfig::kEmptyCell) {
                if (writeRow != r) {
                    m_grid[writeRow][c] = m_grid[r][c];
                    m_grid[r][c] = GameConfig::kEmptyCell;
                }
                --writeRow;
            }
        }
    }
}

int TetrisBoard::ghostRow(const Tetromino& piece, int boardCol) const
{
    int row = 0;
    // Start from current position, move down until collision
    while (canPlace(piece, row + 1, boardCol)) {
        ++row;
    }
    return row;
}

bool TetrisBoard::isGameOver() const
{
    // Check if any block is in the visible top row or buffer zone
    for (int r = 0; r < GameConfig::kBufferRows; ++r) {
        for (int c = 0; c < GameConfig::kBoardWidth; ++c) {
            if (m_grid[r][c] != GameConfig::kEmptyCell) {
                return true;
            }
        }
    }
    return false;
}

QString TetrisBoard::toString() const
{
    QString result;
    for (int r = 0; r < GameConfig::kTotalRows; ++r) {
        for (int c = 0; c < GameConfig::kBoardWidth; ++c) {
            if (m_grid[r][c] == GameConfig::kEmptyCell)
                result += ". ";
            else
                result += QString::number(m_grid[r][c]) + " ";
        }
        result += "\n";
    }
    return result;
}
