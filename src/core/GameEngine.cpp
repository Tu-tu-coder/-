#include "GameEngine.h"
#include "SkillManager.h"
#include <algorithm>

GameEngine::GameEngine(QObject* parent)
    : QObject(parent)
{
    m_scoreCalc = new ScoreCalculator(this);

    // Forward score signals
    connect(m_scoreCalc, &ScoreCalculator::scoreChanged,
            this, &GameEngine::scoreChanged);
    connect(m_scoreCalc, &ScoreCalculator::levelUp,
            this, &GameEngine::levelUp);
}

void GameEngine::setSkillManager(SkillManager* sm)
{
    m_skillManager = sm;
}

void GameEngine::startGame(GameMode mode, const QString& /*skillId*/)
{
    m_board.reset();
    m_bag.reset();
    m_scoreCalc->reset();
    m_mode = mode;
    m_gravityAccum = 0;
    m_lockDelayAccum = 0;
    m_onGround = false;
    m_lockMoveCount = 0;
    stopDAS();

    if (m_skillManager) {
        m_skillManager->reset();
    }

    emit gameModeChanged(m_mode);

    // Spawn first piece
    m_nextPiece = m_bag.nextPiece();
    spawnPiece();

    enterState(GameState::Playing);
}

void GameEngine::restart()
{
    startGame(m_mode);
}

void GameEngine::pause()
{
    if (m_state == GameState::Playing || m_state == GameState::SkillActive) {
        enterState(GameState::Paused);
    }
}

void GameEngine::resume()
{
    if (m_state == GameState::Paused) {
        enterState(GameState::Playing);
    }
}

void GameEngine::gameOver()
{
    enterState(GameState::GameOver);
}

void GameEngine::enterState(GameState newState)
{
    if (m_state == newState) return;
    m_state = newState;
    emit gameStateChanged(m_state);
}

// ============ Main Update ============

void GameEngine::update(qint64 deltaMs)
{
    if (m_state != GameState::Playing && m_state != GameState::SkillActive)
        return;

    // Clamp delta to avoid huge jumps
    qint64 dt = std::min(deltaMs, static_cast<qint64>(200));

    // Update active skill
    if (m_skillManager) {
        m_skillManager->update(dt);
    }

    // Gravity
    int gravInterval = gravityInterval();
    m_gravityAccum += dt;

    while (m_gravityAccum >= gravInterval) {
        m_gravityAccum -= gravInterval;
        auto result = moveDown();
        if (result == MoveResult::Locked) {
            lockCurrentPiece();
            break;
        }
    }

    // Lock delay: if on ground and not moving, lock after delay
    if (m_onGround && m_lockMoveCount >= kMaxLockMoves) {
        lockCurrentPiece();
    }

    // DAS (auto-repeat for held keys)
    updateDAS(dt);

    emit boardChanged();
}

// ============ Piece Spawning ============

void GameEngine::spawnPiece()
{
    m_currentPiece = m_nextPiece;
    m_nextPiece = m_bag.nextPiece();

    // Spawn position: centered horizontally, row 0 (buffer) for 2-row buffer
    m_pieceCol = (GameConfig::kBoardWidth - 4) / 2;
    m_pieceRow = 0; // start in buffer zone

    // Check if spawn position is valid
    if (!m_board.canPlace(m_currentPiece, m_pieceRow, m_pieceCol)) {
        // Try one row up
        if (!m_board.canPlace(m_currentPiece, m_pieceRow - 1, m_pieceCol)) {
            gameOver();
            return;
        }
        m_pieceRow = -1;
    }

    m_onGround = false;
    m_lockDelayAccum = 0;
    m_lockMoveCount = 0;
    emit pieceSpawned();
}

// ============ Movement ============

MoveResult GameEngine::moveLeft()
{
    if (m_board.canPlace(m_currentPiece, m_pieceRow, m_pieceCol - 1)) {
        --m_pieceCol;
        if (m_onGround) {
            ++m_lockMoveCount;
        }
        return MoveResult::OK;
    }
    return MoveResult::Collision;
}

MoveResult GameEngine::moveRight()
{
    if (m_board.canPlace(m_currentPiece, m_pieceRow, m_pieceCol + 1)) {
        ++m_pieceCol;
        if (m_onGround) {
            ++m_lockMoveCount;
        }
        return MoveResult::OK;
    }
    return MoveResult::Collision;
}

MoveResult GameEngine::moveDown()
{
    if (m_board.canPlace(m_currentPiece, m_pieceRow + 1, m_pieceCol)) {
        ++m_pieceRow;
        m_onGround = false;
        m_lockDelayAccum = 0;
        return MoveResult::OK;
    }
    // Can't move down - piece is on ground
    m_onGround = true;
    if (m_lockMoveCount == 0) {
        m_lockDelayAccum = 0;
    }
    return MoveResult::Locked;
}

MoveResult GameEngine::rotateCW()
{
    auto& newShape = m_currentPiece.cwShape();
    // Store current rotation
    int oldRot = m_currentPiece.rotation();

    // Try all wall kick offsets
    auto& kicks = m_currentPiece.wallKickCW()[oldRot];
    for (auto& [dr, dc] : kicks) {
        if (m_board.canPlace(m_currentPiece, m_pieceRow - dr, m_pieceCol + dc)) {
            m_currentPiece.rotateCW();
            m_pieceRow -= dr;
            m_pieceCol += dc;
            if (m_onGround) ++m_lockMoveCount;
            return MoveResult::OK;
        }
    }
    return MoveResult::Collision;
}

MoveResult GameEngine::rotateCCW()
{
    int oldRot = m_currentPiece.rotation();

    auto& kicks = m_currentPiece.wallKickCCW()[oldRot];
    for (auto& [dr, dc] : kicks) {
        if (m_board.canPlace(m_currentPiece, m_pieceRow - dr, m_pieceCol + dc)) {
            m_currentPiece.rotateCCW();
            m_pieceRow -= dr;
            m_pieceCol += dc;
            if (m_onGround) ++m_lockMoveCount;
            return MoveResult::OK;
        }
    }
    return MoveResult::Collision;
}

void GameEngine::hardDrop()
{
    int dropDistance = 0;
    while (m_board.canPlace(m_currentPiece, m_pieceRow + 1, m_pieceCol)) {
        ++m_pieceRow;
        ++dropDistance;
    }
    m_scoreCalc->addHardDrop(dropDistance);
    lockCurrentPiece();
}

void GameEngine::softDrop()
{
    if (moveDown() == MoveResult::Locked) {
        lockCurrentPiece();
    } else {
        m_scoreCalc->addSoftDrop(1);
    }
    // Reset gravity so soft drop doesn't stack with normal gravity
    m_gravityAccum = 0;
}

// ============ Lock & Clear ============

void GameEngine::lockCurrentPiece()
{
    auto clearedRows = m_board.lockPiece(m_currentPiece, m_pieceRow, m_pieceCol);

    if (m_board.isGameOver()) {
        emit boardChanged();
        gameOver();
        return;
    }

    handleLineClears(clearedRows);
    emit pieceLocked();
    spawnPiece();
}

void GameEngine::handleLineClears(const std::vector<int>& clearedRows)
{
    int nCleared = static_cast<int>(clearedRows.size());

    if (nCleared > 0) {
        // Score
        int points = m_scoreCalc->addLineClear(nCleared);
        emit linesCleared(nCleared);

        // Energy
        if (m_skillManager) {
            int energyGain = 0;
            switch (nCleared) {
                case 1: energyGain = GameConfig::kEnergySingle; break;
                case 2: energyGain = GameConfig::kEnergyDouble; break;
                case 3: energyGain = GameConfig::kEnergyTriple; break;
                case 4: energyGain = GameConfig::kEnergyTetris; break;
            }
            // Combo bonus
            int combo = m_scoreCalc->combo();
            if (combo > 1) {
                energyGain += GameConfig::kEnergyComboBonus * (combo - 1);
            }
            m_skillManager->addEnergy(energyGain);
        }

        if (nCleared >= 4) {
            emit comboAchieved(m_scoreCalc->combo());
        }
    } else {
        m_scoreCalc->resetCombo();
    }
}

// ============ Input ============

void GameEngine::processInput(GameAction action, bool pressed)
{
    m_keyState[static_cast<int>(action)] = pressed;

    if (m_state != GameState::Playing && m_state != GameState::SkillActive) {
        // Only allow pause in non-playing states (except for pause itself)
        if (action == GameAction::Pause && pressed) {
            if (m_state == GameState::Paused) resume();
            else if (m_state == GameState::Playing || m_state == GameState::SkillActive) pause();
        }
        return;
    }

    if (!pressed) {
        // Key release - stop DAS for movement keys
        if (action == GameAction::MoveLeft || action == GameAction::MoveRight) {
            if (m_dasActive && m_dasAction == action) {
                stopDAS();
            }
        }
        return;
    }

    // Key pressed
    switch (action) {
        case GameAction::MoveLeft:
            moveLeft();
            startDAS(GameAction::MoveLeft);
            break;
        case GameAction::MoveRight:
            moveRight();
            startDAS(GameAction::MoveRight);
            break;
        case GameAction::RotateCW:
            rotateCW();
            break;
        case GameAction::RotateCCW:
            rotateCCW();
            break;
        case GameAction::SoftDrop:
            softDrop();
            break;
        case GameAction::HardDrop:
            hardDrop();
            break;
        case GameAction::ActivateSkill:
            if (m_skillManager && isSkillsEnabled()) {
                if (m_skillManager->tryActivate(m_board)) {
                    // Skill activation will emit its own signals
                }
            }
            break;
        case GameAction::Pause:
            pause();
            break;
        default:
            break;
    }

    emit boardChanged();
}

// ============ DAS ============

void GameEngine::startDAS(GameAction action)
{
    m_dasActive = true;
    m_dasCharged = false;
    m_dasAccum = 0;
    m_dasAction = action;
}

void GameEngine::stopDAS()
{
    m_dasActive = false;
}

void GameEngine::updateDAS(qint64 deltaMs)
{
    if (!m_dasActive) return;

    m_dasAccum += deltaMs;

    if (!m_dasCharged && m_dasAccum >= m_dasDelay) {
        // Initial delay passed, start repeating
        m_dasCharged = true;
        m_dasAccum -= m_dasDelay;
        // Fire first repeat immediately
        if (m_dasAction == GameAction::MoveLeft) moveLeft();
        else if (m_dasAction == GameAction::MoveRight) moveRight();
    } else if (m_dasCharged) {
        while (m_dasAccum >= m_arrInterval) {
            m_dasAccum -= m_arrInterval;
            if (m_dasAction == GameAction::MoveLeft) moveLeft();
            else if (m_dasAction == GameAction::MoveRight) moveRight();
        }
    }
}

// ============ Accessors ============

int GameEngine::gravityInterval() const
{
    int base = GameConfig::kBaseGravityMs - (m_scoreCalc->level() - 1) * GameConfig::kGravityReductionPerLevel;
    base = std::max(base, GameConfig::kMinGravityMs);

    // Skill override (e.g. TimeFreeze slows gravity)
    if (m_skillManager && m_skillManager->isSkillActive()) {
        base = static_cast<int>(base / std::max(0.1f, m_skillManager->gravityMultiplier()));
    }

    return base;
}

int GameEngine::ghostRow() const
{
    return m_board.ghostRow(m_currentPiece, m_pieceCol);
}

int GameEngine::score() const { return m_scoreCalc->score(); }
int GameEngine::level() const { return m_scoreCalc->level(); }
int GameEngine::totalLines() const { return m_scoreCalc->totalLines(); }
int GameEngine::combo() const { return m_scoreCalc->combo(); }
