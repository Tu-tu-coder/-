#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <QObject>
#include "GameConfig.h"
#include "Tetromino.h"
#include "TetrisBoard.h"
#include "RandomBag.h"
#include "ScoreCalculator.h"
#include "MoveResult.h"

// Forward declarations
class SkillManager;

enum class GameState {
    Idle,
    Playing,
    Paused,
    SkillActive,  // skill is currently in effect
    GameOver
};

enum class GameAction {
    MoveLeft,
    MoveRight,
    RotateCW,
    RotateCCW,
    SoftDrop,
    HardDrop,
    HoldPiece,
    ActivateSkill,
    ActivateSkillSlot1,
    ActivateSkillSlot2,
    ActivateSkillSlot3,
    ActivateSkillSlot4,
    Pause,
    Count
};

enum class GameMode {
    Classic,           // No skills
    InfiniteChallenge, // Skills enabled
    Battle             // 2-player (future)
};

class GameEngine : public QObject {
    Q_OBJECT
public:
    explicit GameEngine(QObject* parent = nullptr);

    // Game lifecycle
    void startGame(GameMode mode);
    void restart();
    void pause();
    void resume();
    void gameOver();

    // Main update tick (called from timer, delta in ms)
    void update(qint64 deltaMs);

    // Input handling
    void processInput(GameAction action, bool pressed);

    // State accessors
    GameState state() const { return m_state; }
    GameMode mode() const { return m_mode; }
    const TetrisBoard& board() const { return m_board; }
    TetrisBoard& board() { return m_board; }
    const Tetromino& currentPiece() const { return m_currentPiece; }
    const Tetromino& nextPiece() const { return m_nextPiece; }
    int score() const;
    int level() const;
    int totalLines() const;
    int combo() const;

    // Ghost piece position
    int ghostRow() const;

    // Piece position
    int pieceRow() const { return m_pieceRow; }
    int pieceCol() const { return m_pieceCol; }

    // Gravity interval for current level / skill state
    int gravityInterval() const;

    // Skill access
    SkillManager* skillManager() const { return m_skillManager; }
    void setSkillManager(SkillManager* sm);

    // DAS (Delayed Auto Shift) configuration
    void setDASDelay(int ms) { m_dasDelay = ms; }
    void setARRInterval(int ms) { m_arrInterval = ms; }

    // Mode configuration
    bool isSkillsEnabled() const { return m_mode == GameMode::InfiniteChallenge; }

signals:
    void boardChanged();
    void scoreChanged(int score, int level, int lines);
    void levelUp(int newLevel);
    void comboAchieved(int comboCount);
    void linesCleared(int count);
    void pieceLocked();
    void pieceSpawned();
    void gameStateChanged(GameState state);
    void gameModeChanged(GameMode mode);
    void energyChanged(int current, int max);
    void skillActivated(const QString& skillId);
    void skillDeactivated(const QString& skillId);

private:
    void enterState(GameState newState);
    void spawnPiece();
    MoveResult moveLeft();
    MoveResult moveRight();
    MoveResult moveDown();
    MoveResult rotateCW();
    MoveResult rotateCCW();
    void hardDrop();
    void lockCurrentPiece();
    void handleLineClears(const std::vector<int>& clearedRows);

    // DAS handling
    void updateDAS(qint64 deltaMs);
    void startDAS(GameAction action);
    void stopDAS();

    // Board & pieces
    TetrisBoard m_board;
    RandomBag m_bag;
    Tetromino m_currentPiece;
    Tetromino m_nextPiece;
    int m_pieceRow = 0;
    int m_pieceCol = 0;

    // Score & progress
    ScoreCalculator* m_scoreCalc;
    SkillManager* m_skillManager = nullptr;

    // State
    GameState m_state = GameState::Idle;
    GameMode m_mode = GameMode::InfiniteChallenge;

    // Gravity
    qint64 m_gravityAccum = 0;
    qint64 m_lockDelayAccum = 0;
    bool m_onGround = false;
    bool m_softDropping = false; // true while down key is held
    int m_lockMoveCount = 0;
    static constexpr int kMaxLockMoves = 15; // max moves/resets before forced lock

    // DAS (Delayed Auto Shift)
    int m_dasDelay = GameConfig::kDASDelay;
    int m_arrInterval = GameConfig::kARRInterval;
    qint64 m_dasAccum = 0;
    GameAction m_dasAction = GameAction::MoveLeft; // current DAS direction
    bool m_dasActive = false;
    bool m_dasCharged = false; // initial delay passed

    // Key state tracking
    bool m_keyState[static_cast<int>(GameAction::Count)] = {false};
};

#endif // GAMEENGINE_H
