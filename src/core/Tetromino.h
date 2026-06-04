#ifndef TETROMINO_H
#define TETROMINO_H

#include "GameConfig.h"
#include <array>
#include <vector>

// SRS (Super Rotation System) tetromino
// Each piece has 4 rotation states stored as 4x4 bitmasks
class Tetromino {
public:
    using Shape = std::array<std::array<int, 4>, 4>; // 4x4 grid

    Tetromino();
    explicit Tetromino(GameConfig::PieceType type);

    GameConfig::PieceType type() const { return m_type; }
    QColor color() const { return m_color; }

    // Current rotation state (0-3)
    int rotation() const { return m_rotation; }
    void setRotation(int rot) { m_rotation = rot & 3; }

    // Get the 4x4 shape for the current rotation
    const Shape& currentShape() const { return m_shapes[m_rotation]; }

    // Rotate clockwise / counter-clockwise and return the resulting shape
    const Shape& cwShape() const { return m_shapes[(m_rotation + 1) & 3]; }
    const Shape& ccwShape() const { return m_shapes[(m_rotation + 3) & 3]; }

    void rotateCW() { m_rotation = (m_rotation + 1) & 3; }
    void rotateCCW() { m_rotation = (m_rotation + 3) & 3; }

    // Get cells occupied (relative to piece origin) for current rotation
    // Returns vector of {row, col} offsets (0..3)
    std::vector<std::pair<int, int>> occupiedCells() const;

    // Get cells for a specific rotation
    static std::vector<std::pair<int, int>> cellsFromShape(const Shape& shape);

    // SRS wall kick offset data
    // kickData[fromRotation][toRotation][testIndex] = {rowOffset, colOffset}
    using KickOffset = std::pair<int, int>;
    using KickTable = std::array<std::array<KickOffset, 5>, 4>; // 4 rotations x 5 tests

    const KickTable& wallKickCW() const;   // CW kicks for this piece
    const KickTable& wallKickCCW() const;  // CCW kicks for this piece

    // Initialize all 7 piece shapes (SRS standard)
    static void initShapes();

private:
    GameConfig::PieceType m_type;
    QColor m_color;
    int m_rotation = 0;
    std::array<Shape, 4> m_shapes; // 4 rotation states

    // Global shape storage — initialized once
    static bool s_shapesInitialized;
    static std::array<std::array<Shape, 4>, GameConfig::kPieceCount> s_allShapes;
    // Kick data for I-piece and JLSZT-piece (different tables)
    static KickTable s_kickI_CW, s_kickI_CCW;
    static KickTable s_kickStd_CW, s_kickStd_CCW;
};

#endif // TETROMINO_H
