#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <QColor>
#include <QString>

namespace GameConfig {

// Board dimensions
constexpr int kBoardWidth = 10;
constexpr int kBoardHeight = 20;
constexpr int kBufferRows = 2;       // hidden rows above visible area
constexpr int kTotalRows = kBoardHeight + kBufferRows; // 22

// Piece types
enum class PieceType {
    I = 0, O, T, S, Z, J, L, COUNT
};
constexpr int kPieceCount = 7;

// Timing (milliseconds)
constexpr int kFrameInterval = 16;   // ~60 FPS
constexpr int kBaseGravityMs = 800;  // level 1 drop interval
constexpr int kMinGravityMs = 50;    // fastest drop
constexpr int kDASDelay = 167;       // delayed auto shift
constexpr int kARRInterval = 33;     // auto repeat rate
constexpr int kLockDelayMs = 500;    // time before piece locks after landing
constexpr int kSoftDropInterval = 50;

// Scoring (multiplied by level)
constexpr int kScoreSingle = 100;
constexpr int kScoreDouble = 300;
constexpr int kScoreTriple = 500;
constexpr int kScoreTetris = 800;
constexpr int kScoreCombo = 50;      // per combo count
constexpr int kScoreSoftDrop = 1;    // per cell
constexpr int kScoreHardDrop = 2;    // per cell

// Level progression
constexpr int kLinesPerLevel = 10;
constexpr int kGravityReductionPerLevel = 20; // ms faster per level
constexpr int kMaxLevel = 30;

// Energy system
constexpr int kEnergyMax = 100;
constexpr int kEnergySingle = 20;
constexpr int kEnergyDouble = 35;
constexpr int kEnergyTriple = 50;
constexpr int kEnergyTetris = 80;
constexpr int kEnergyComboBonus = 5;

// Skill duration (ms)
constexpr int kTimeFreezeDuration = 6000;
constexpr float kTimeFreezeSlowFactor = 0.2f;

// Piece colors (bright cartoon style)
inline QColor pieceColor(PieceType type) {
    switch (type) {
        case PieceType::I: return QColor(0, 240, 240);    // Cyan
        case PieceType::O: return QColor(240, 240, 0);    // Yellow
        case PieceType::T: return QColor(160, 0, 240);    // Purple
        case PieceType::S: return QColor(0, 240, 0);      // Green
        case PieceType::Z: return QColor(240, 0, 0);      // Red
        case PieceType::J: return QColor(0, 0, 240);      // Blue
        case PieceType::L: return QColor(240, 160, 0);    // Orange
        default: return QColor(128, 128, 128);
    }
}

inline QString pieceName(PieceType type) {
    switch (type) {
        case PieceType::I: return "I";
        case PieceType::O: return "O";
        case PieceType::T: return "T";
        case PieceType::S: return "S";
        case PieceType::Z: return "Z";
        case PieceType::J: return "J";
        case PieceType::L: return "L";
        default: return "?";
    }
}

// Board colors
constexpr int kEmptyCell = -1;

} // namespace GameConfig

#endif // GAMECONFIG_H
