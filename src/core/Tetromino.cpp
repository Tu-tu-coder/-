#include "Tetromino.h"
#include <algorithm>

// Static member initialization
bool Tetromino::s_shapesInitialized = false;
std::array<std::array<Tetromino::Shape, 4>, GameConfig::kPieceCount> Tetromino::s_allShapes;

// Helper to make a KickOffset
static constexpr Tetromino::KickOffset K(int dr, int dc) {
    return {dr, dc};
}

// Pre-computed kick tables using constexpr helper
Tetromino::KickTable Tetromino::s_kickI_CW = {{
    {{ K(0,0), K(-2,0), K( 1,0), K(-2,-1), K( 1, 2) }},  // 0→1
    {{ K(0,0), K(-1,0), K( 2,0), K(-1, 2), K( 2,-1) }},  // 1→2
    {{ K(0,0), K( 2,0), K(-1,0), K( 2, 1), K(-1,-2) }},  // 2→3
    {{ K(0,0), K( 1,0), K(-2,0), K( 1,-2), K(-2, 1) }},  // 3→0
}};

Tetromino::KickTable Tetromino::s_kickI_CCW = {{
    {{ K(0,0), K(-1,0), K( 2,0), K(-1, 2), K( 2,-1) }},  // 0→3
    {{ K(0,0), K( 2,0), K(-1,0), K( 2, 1), K(-1,-2) }},  // 1→0
    {{ K(0,0), K( 1,0), K(-2,0), K( 1,-2), K(-2, 1) }},  // 2→1
    {{ K(0,0), K(-2,0), K( 1,0), K(-2,-1), K( 1, 2) }},  // 3→2
}};

Tetromino::KickTable Tetromino::s_kickStd_CW = {{
    {{ K(0,0), K(-1,0), K(-1, 1), K( 0,-2), K(-1,-2) }},  // 0→1
    {{ K(0,0), K( 1,0), K( 1,-1), K( 0, 2), K( 1, 2) }},  // 1→2
    {{ K(0,0), K( 1,0), K( 1, 1), K( 0,-2), K( 1,-2) }},  // 2→3
    {{ K(0,0), K(-1,0), K(-1,-1), K( 0, 2), K(-1, 2) }},  // 3→0
}};

Tetromino::KickTable Tetromino::s_kickStd_CCW = {{
    {{ K(0,0), K( 1,0), K( 1, 1), K( 0,-2), K( 1,-2) }},  // 0→3
    {{ K(0,0), K( 1,0), K( 1,-1), K( 0, 2), K( 1, 2) }},  // 1→0
    {{ K(0,0), K(-1,0), K(-1, 1), K( 0,-2), K(-1,-2) }},  // 2→1
    {{ K(0,0), K(-1,0), K(-1,-1), K( 0, 2), K(-1, 2) }},  // 3→2
}};

// SRS rotation states: shape[rotation][row][col], 1 = filled, 0 = empty
void Tetromino::initShapes()
{
    if (s_shapesInitialized) return;
    s_shapesInitialized = true;

    using PT = GameConfig::PieceType;

    // I piece
    {
        auto& shapes = s_allShapes[static_cast<int>(PT::I)];
        shapes[0] = {{
            {{0, 0, 0, 0}},
            {{1, 1, 1, 1}},
            {{0, 0, 0, 0}},
            {{0, 0, 0, 0}}
        }};
        shapes[1] = {{
            {{0, 0, 1, 0}},
            {{0, 0, 1, 0}},
            {{0, 0, 1, 0}},
            {{0, 0, 1, 0}}
        }};
        shapes[2] = {{
            {{0, 0, 0, 0}},
            {{0, 0, 0, 0}},
            {{1, 1, 1, 1}},
            {{0, 0, 0, 0}}
        }};
        shapes[3] = {{
            {{0, 1, 0, 0}},
            {{0, 1, 0, 0}},
            {{0, 1, 0, 0}},
            {{0, 1, 0, 0}}
        }};
    }

    // O piece (all rotations identical)
    {
        auto& shapes = s_allShapes[static_cast<int>(PT::O)];
        for (int r = 0; r < 4; ++r) {
            shapes[r] = {{
                {{0, 0, 0, 0}},
                {{0, 1, 1, 0}},
                {{0, 1, 1, 0}},
                {{0, 0, 0, 0}}
            }};
        }
    }

    // T piece
    {
        auto& shapes = s_allShapes[static_cast<int>(PT::T)];
        shapes[0] = {{
            {{0, 0, 0, 0}},
            {{1, 1, 1, 0}},
            {{0, 1, 0, 0}},
            {{0, 0, 0, 0}}
        }};
        shapes[1] = {{
            {{0, 1, 0, 0}},
            {{1, 1, 0, 0}},
            {{0, 1, 0, 0}},
            {{0, 0, 0, 0}}
        }};
        shapes[2] = {{
            {{0, 1, 0, 0}},
            {{1, 1, 1, 0}},
            {{0, 0, 0, 0}},
            {{0, 0, 0, 0}}
        }};
        shapes[3] = {{
            {{0, 1, 0, 0}},
            {{0, 1, 1, 0}},
            {{0, 1, 0, 0}},
            {{0, 0, 0, 0}}
        }};
    }

    // S piece
    {
        auto& shapes = s_allShapes[static_cast<int>(PT::S)];
        shapes[0] = {{
            {{0, 0, 0, 0}},
            {{0, 1, 1, 0}},
            {{1, 1, 0, 0}},
            {{0, 0, 0, 0}}
        }};
        shapes[1] = {{
            {{1, 0, 0, 0}},
            {{1, 1, 0, 0}},
            {{0, 1, 0, 0}},
            {{0, 0, 0, 0}}
        }};
        shapes[2] = shapes[0];
        shapes[3] = shapes[1];
    }

    // Z piece
    {
        auto& shapes = s_allShapes[static_cast<int>(PT::Z)];
        shapes[0] = {{
            {{0, 0, 0, 0}},
            {{1, 1, 0, 0}},
            {{0, 1, 1, 0}},
            {{0, 0, 0, 0}}
        }};
        shapes[1] = {{
            {{0, 1, 0, 0}},
            {{1, 1, 0, 0}},
            {{1, 0, 0, 0}},
            {{0, 0, 0, 0}}
        }};
        shapes[2] = shapes[0];
        shapes[3] = shapes[1];
    }

    // J piece
    {
        auto& shapes = s_allShapes[static_cast<int>(PT::J)];
        shapes[0] = {{
            {{1, 0, 0, 0}},
            {{1, 1, 1, 0}},
            {{0, 0, 0, 0}},
            {{0, 0, 0, 0}}
        }};
        shapes[1] = {{
            {{0, 1, 1, 0}},
            {{0, 1, 0, 0}},
            {{0, 1, 0, 0}},
            {{0, 0, 0, 0}}
        }};
        shapes[2] = {{
            {{0, 0, 0, 0}},
            {{1, 1, 1, 0}},
            {{0, 0, 1, 0}},
            {{0, 0, 0, 0}}
        }};
        shapes[3] = {{
            {{0, 1, 0, 0}},
            {{0, 1, 0, 0}},
            {{1, 1, 0, 0}},
            {{0, 0, 0, 0}}
        }};
    }

    // L piece
    {
        auto& shapes = s_allShapes[static_cast<int>(PT::L)];
        shapes[0] = {{
            {{0, 0, 1, 0}},
            {{1, 1, 1, 0}},
            {{0, 0, 0, 0}},
            {{0, 0, 0, 0}}
        }};
        shapes[1] = {{
            {{0, 1, 0, 0}},
            {{0, 1, 0, 0}},
            {{0, 1, 1, 0}},
            {{0, 0, 0, 0}}
        }};
        shapes[2] = {{
            {{0, 0, 0, 0}},
            {{1, 1, 1, 0}},
            {{1, 0, 0, 0}},
            {{0, 0, 0, 0}}
        }};
        shapes[3] = {{
            {{1, 1, 0, 0}},
            {{0, 1, 0, 0}},
            {{0, 1, 0, 0}},
            {{0, 0, 0, 0}}
        }};
    }
}

Tetromino::Tetromino()
    : m_type(GameConfig::PieceType::I)
    , m_color(GameConfig::pieceColor(GameConfig::PieceType::I))
    , m_rotation(0)
{
    initShapes();
}

Tetromino::Tetromino(GameConfig::PieceType type)
    : m_type(type)
    , m_color(GameConfig::pieceColor(type))
    , m_rotation(0)
{
    initShapes();
    m_shapes = s_allShapes[static_cast<int>(type)];
}

std::vector<std::pair<int, int>> Tetromino::cellsFromShape(const Shape& shape)
{
    std::vector<std::pair<int, int>> cells;
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (shape[r][c]) {
                cells.emplace_back(r, c);
            }
        }
    }
    return cells;
}

std::vector<std::pair<int, int>> Tetromino::occupiedCells() const
{
    return cellsFromShape(m_shapes[m_rotation]);
}

const Tetromino::KickTable& Tetromino::wallKickCW() const
{
    if (m_type == GameConfig::PieceType::I) return s_kickI_CW;
    return s_kickStd_CW;
}

const Tetromino::KickTable& Tetromino::wallKickCCW() const
{
    if (m_type == GameConfig::PieceType::I) return s_kickI_CCW;
    return s_kickStd_CCW;
}
