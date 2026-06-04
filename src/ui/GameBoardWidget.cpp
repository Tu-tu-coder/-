#include "GameBoardWidget.h"
#include "../core/GameEngine.h"
#include "../core/GameConfig.h"
#include "../core/SkillManager.h"
#include "effects/ScreenFlash.h"
#include "effects/ScreenShake.h"
#include "effects/ParticleSystem.h"
#include <QPainter>
#include <QKeyEvent>
#include <QTimer>
#include <algorithm>

GameBoardWidget::GameBoardWidget(GameEngine* engine, QWidget* parent)
    : QWidget(parent)
    , m_engine(engine)
    , m_flash(new ScreenFlash(this))
    , m_shake(new ScreenShake(this))
    , m_particles(new ParticleSystem(this))
{
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(300, 600);

    // Connect engine signals to repaint
    if (m_engine) {
        connect(m_engine, &GameEngine::boardChanged,
                this, QOverload<>::of(&QWidget::update));
        connect(m_engine, &GameEngine::linesCleared, this, [this](int count) {
            Q_UNUSED(count);
            m_lineClearTimer.start();
            // Flash animation will be handled in paintEvent
        });
        connect(m_engine, &GameEngine::skillActivated, this, [this](const QString& name) {
            if (name.contains("时间凝滞")) {
                m_flash->flash(QColor(60, 160, 255, 80), 400);
            } else if (name.contains("大地震")) {
                m_shake->shake(12.0f, 500);
                m_flash->flash(QColor(180, 100, 40, 100), 300);
            }
            m_particles->burst(QPointF(width() / 2.0, height() / 2.0),
                               m_engine->skillManager()->skillColor(
                                   m_engine->skillManager()->equippedSkillId()),
                               20);
        });
    }
}

void GameBoardWidget::setEngine(GameEngine* engine)
{
    m_engine = engine;
}

QSize GameBoardWidget::minimumSizeHint() const
{
    return QSize(300, 600);
}

QSize GameBoardWidget::sizeHint() const
{
    return QSize(350, 700);
}

int GameBoardWidget::cellSize() const
{
    int w = (width() - 40) / GameConfig::kBoardWidth;
    int h = (height() - 40) / GameConfig::kBoardHeight;
    return std::min(w, h);
}

int GameBoardWidget::boardPixelWidth() const
{
    return cellSize() * GameConfig::kBoardWidth;
}

int GameBoardWidget::boardPixelHeight() const
{
    return cellSize() * GameConfig::kBoardHeight;
}

// ============ Paint ============

void GameBoardWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    // Calculate board position (centered)
    int cs = cellSize();
    m_offsetX = (width() - cs * GameConfig::kBoardWidth) / 2;
    m_offsetY = (height() - cs * GameConfig::kBoardHeight) / 2;

    // Apply screen shake
    if (m_shake->isShaking()) {
        auto offset = m_shake->currentOffset();
        p.translate(offset.first, offset.second);
    }

    // Background
    p.fillRect(rect(), QColor(26, 26, 46));

    // Board background
    QRect boardRect(m_offsetX, m_offsetY, boardPixelWidth(), boardPixelHeight());
    p.fillRect(boardRect, QColor(10, 10, 25));
    p.setPen(QPen(QColor(60, 60, 100), 2));
    p.drawRect(boardRect);

    drawGrid(p);
    drawGhost(p);
    drawActivePiece(p);
    drawBoard(p);

    // Screen flash overlay
    m_flash->draw(p, rect());

    // Particles
    m_particles->draw(p);

    // Pause overlay
    if (m_engine && m_engine->state() == GameState::Paused) {
        p.fillRect(rect(), QColor(0, 0, 0, 150));
        p.setPen(Qt::white);
        QFont font = p.font();
        font.setPointSize(28);
        font.setBold(true);
        p.setFont(font);
        p.drawText(rect(), Qt::AlignCenter, "暂停");
    }
}

void GameBoardWidget::drawGrid(QPainter& p)
{
    int cs = cellSize();
    p.setPen(QPen(QColor(40, 40, 60), 1));

    for (int r = 0; r <= GameConfig::kBoardHeight; ++r) {
        p.drawLine(m_offsetX, m_offsetY + r * cs,
                   m_offsetX + boardPixelWidth(), m_offsetY + r * cs);
    }
    for (int c = 0; c <= GameConfig::kBoardWidth; ++c) {
        p.drawLine(m_offsetX + c * cs, m_offsetY,
                   m_offsetX + c * cs, m_offsetY + boardPixelHeight());
    }
}

void GameBoardWidget::drawCell(QPainter& p, int col, int row,
                                const QColor& color, float alpha)
{
    int cs = cellSize();
    int x = m_offsetX + col * cs;
    int y = m_offsetY + row * cs;
    int inset = 1;

    QRect cellRect(x + inset, y + inset, cs - 2 * inset, cs - 2 * inset);

    p.setOpacity(static_cast<double>(alpha));
    p.setBrush(color);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(cellRect, 2, 2);

    // 3D bevel effect: lighter top-left edge, darker bottom-right
    p.setPen(QPen(color.lighter(150), 1));
    p.drawLine(cellRect.topLeft(), cellRect.topRight());
    p.drawLine(cellRect.topLeft(), cellRect.bottomLeft());

    p.setPen(QPen(color.darker(150), 1));
    p.drawLine(cellRect.bottomLeft(), cellRect.bottomRight());
    p.drawLine(cellRect.topRight(), cellRect.bottomRight());

    p.setOpacity(1.0);
}

void GameBoardWidget::drawGhost(QPainter& p)
{
    if (!m_engine) return;
    if (m_engine->state() != GameState::Playing &&
        m_engine->state() != GameState::SkillActive) return;

    int ghostY = m_engine->ghostRow();
    int pieceRow = m_engine->pieceRow();

    // Only draw ghost if it's below the current piece
    if (ghostY <= pieceRow) return;

    auto& piece = m_engine->currentPiece();
    auto cells = piece.occupiedCells();
    QColor color = piece.color();

    for (auto& [r, c] : cells) {
        int absR = ghostY + r - GameConfig::kBufferRows;
        int absC = m_engine->pieceCol() + c;
        if (absR >= 0 && absR < GameConfig::kBoardHeight && absC >= 0 && absC < GameConfig::kBoardWidth) {
            drawCell(p, absC, absR, color, 0.25f);
        }
    }
}

void GameBoardWidget::drawActivePiece(QPainter& p)
{
    if (!m_engine) return;
    if (m_engine->state() != GameState::Playing &&
        m_engine->state() != GameState::SkillActive) return;

    auto& piece = m_engine->currentPiece();
    auto cells = piece.occupiedCells();
    QColor color = piece.color();

    for (auto& [r, c] : cells) {
        int absR = m_engine->pieceRow() + r - GameConfig::kBufferRows;
        int absC = m_engine->pieceCol() + c;
        if (absR >= 0 && absR < GameConfig::kBoardHeight && absC >= 0 && absC < GameConfig::kBoardWidth) {
            drawCell(p, absC, absR, color, 1.0f);
        }
    }
}

void GameBoardWidget::drawBoard(QPainter& p)
{
    if (!m_engine) return;

    auto& board = m_engine->board();

    for (int r = 0; r < GameConfig::kBoardHeight; ++r) {
        for (int c = 0; c < GameConfig::kBoardWidth; ++c) {
            int cell = board.cellAtVisible(r, c);
            if (cell != GameConfig::kEmptyCell) {
                QColor color = GameConfig::pieceColor(
                    static_cast<GameConfig::PieceType>(cell));
                drawCell(p, c, r, color, 1.0f);
            }
        }
    }
}

void GameBoardWidget::drawEffects(QPainter& p)
{
    Q_UNUSED(p);
    // Additional effects drawn in paintEvent via flash/shake/particles
}

// ============ Keyboard Input ============

GameAction GameBoardWidget::keyToAction(int key) const
{
    switch (key) {
        case Qt::Key_Left:
        case Qt::Key_A:
            return GameAction::MoveLeft;
        case Qt::Key_Right:
        case Qt::Key_D:
            return GameAction::MoveRight;
        case Qt::Key_Up:
        case Qt::Key_W:
            return GameAction::RotateCW;
        case Qt::Key_Z:
            return GameAction::RotateCCW;
        case Qt::Key_Down:
        case Qt::Key_S:
            return GameAction::SoftDrop;
        case Qt::Key_Space:
            return GameAction::HardDrop;
        case Qt::Key_V:
        case Qt::Key_E:
            return GameAction::ActivateSkill;
        case Qt::Key_Escape:
        case Qt::Key_P:
            return GameAction::Pause;
        default:
            return GameAction::Count; // invalid
    }
}

void GameBoardWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->isAutoRepeat()) {
        // Qt sends auto-repeat for held keys; we handle DAS ourselves
        return;
    }

    GameAction action = keyToAction(event->key());
    if (action != GameAction::Count && m_engine) {
        m_engine->processInput(action, true);
    }

    QWidget::keyPressEvent(event);
}

void GameBoardWidget::keyReleaseEvent(QKeyEvent* event)
{
    if (event->isAutoRepeat()) return;

    GameAction action = keyToAction(event->key());
    if (action != GameAction::Count && m_engine) {
        m_engine->processInput(action, false);
    }

    QWidget::keyReleaseEvent(event);
}
