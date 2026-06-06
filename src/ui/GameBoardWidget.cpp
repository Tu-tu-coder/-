#include "GameBoardWidget.h"

#include "../core/GameConfig.h"
#include "../core/GameEngine.h"
#include "../core/SkillManager.h"
#include "effects/ParticleSystem.h"
#include "effects/ScreenFlash.h"
#include "effects/ScreenShake.h"

#include <QKeyEvent>
#include <QPainter>
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

    if (m_engine) {
        connect(m_engine, &GameEngine::boardChanged, this, QOverload<>::of(&QWidget::update));
        connect(m_engine, &GameEngine::linesCleared, this, [this](int count) {
            Q_UNUSED(count);
            m_lineClearTimer.start();
        });
        connect(m_engine, &GameEngine::skillActivated, this, [this](const QString& skillId) {
            if (skillId == QStringLiteral("time_freeze") || skillId == QStringLiteral("gravity_break")) {
                m_flash->flash(QColor(60, 160, 255, 80), 400);
            } else if (skillId == QStringLiteral("earthquake")) {
                m_shake->shake(12.0f, 500);
                m_flash->flash(QColor(180, 100, 40, 100), 300);
            } else if (skillId == QStringLiteral("purge_wave")) {
                m_shake->shake(16.0f, 650);
                m_flash->flash(QColor(255, 90, 120, 120), 450);
            } else {
                m_flash->flash(QColor(80, 220, 180, 80), 250);
            }
            m_particles->burst(QPointF(width() / 2.0, height() / 2.0),
                               m_engine->skillManager()->skillColor(skillId),
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
    const int w = (width() - 40) / GameConfig::kBoardWidth;
    const int h = (height() - 40) / GameConfig::kBoardHeight;
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

void GameBoardWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    const int cs = cellSize();
    m_offsetX = (width() - cs * GameConfig::kBoardWidth) / 2;
    m_offsetY = (height() - cs * GameConfig::kBoardHeight) / 2;

    if (m_shake->isShaking()) {
        auto offset = m_shake->currentOffset();
        p.translate(offset.first, offset.second);
    }

    p.fillRect(rect(), QColor(26, 26, 46));

    QRect boardRect(m_offsetX, m_offsetY, boardPixelWidth(), boardPixelHeight());
    p.fillRect(boardRect, QColor(10, 10, 25));
    p.setPen(QPen(QColor(60, 60, 100), 2));
    p.drawRect(boardRect);

    drawGrid(p);
    drawGhost(p);
    drawActivePiece(p);
    drawBoard(p);

    m_flash->draw(p, rect());
    m_particles->draw(p);

    if (m_engine && m_engine->state() == GameState::Paused) {
        p.fillRect(rect(), QColor(0, 0, 0, 150));
        p.setPen(Qt::white);
        QFont font = p.font();
        font.setPointSize(28);
        font.setBold(true);
        p.setFont(font);
        p.drawText(rect(), Qt::AlignCenter, QStringLiteral("暂停"));
    }
}

void GameBoardWidget::drawGrid(QPainter& p)
{
    const int cs = cellSize();
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

void GameBoardWidget::drawCell(QPainter& p, int col, int row, const QColor& color, float alpha)
{
    const int cs = cellSize();
    const int x = m_offsetX + col * cs;
    const int y = m_offsetY + row * cs;
    const int inset = 1;

    QRect cellRect(x + inset, y + inset, cs - 2 * inset, cs - 2 * inset);

    p.setOpacity(static_cast<double>(alpha));
    p.setBrush(color);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(cellRect, 2, 2);

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

    const int ghostY = m_engine->ghostRow();
    const int pieceRow = m_engine->pieceRow();
    if (ghostY <= pieceRow) return;

    const auto& piece = m_engine->currentPiece();
    auto cells = piece.occupiedCells();
    const QColor color = piece.color();

    for (auto& [r, c] : cells) {
        const int absR = ghostY + r - GameConfig::kBufferRows;
        const int absC = m_engine->pieceCol() + c;
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

    const auto& piece = m_engine->currentPiece();
    auto cells = piece.occupiedCells();
    const QColor color = piece.color();

    for (auto& [r, c] : cells) {
        const int absR = m_engine->pieceRow() + r - GameConfig::kBufferRows;
        const int absC = m_engine->pieceCol() + c;
        if (absR >= 0 && absR < GameConfig::kBoardHeight && absC >= 0 && absC < GameConfig::kBoardWidth) {
            drawCell(p, absC, absR, color, 1.0f);
        }
    }
}

void GameBoardWidget::drawBoard(QPainter& p)
{
    if (!m_engine) return;

    const auto& board = m_engine->board();
    for (int r = 0; r < GameConfig::kBoardHeight; ++r) {
        for (int c = 0; c < GameConfig::kBoardWidth; ++c) {
            const int cell = board.cellAtVisible(r, c);
            if (cell != GameConfig::kEmptyCell) {
                drawCell(p, c, r, GameConfig::pieceColor(static_cast<GameConfig::PieceType>(cell)), 1.0f);
            }
        }
    }
}

void GameBoardWidget::drawEffects(QPainter& p)
{
    Q_UNUSED(p);
}

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
        case Qt::Key_1:
            return GameAction::ActivateSkillSlot1;
        case Qt::Key_2:
            return GameAction::ActivateSkillSlot2;
        case Qt::Key_3:
            return GameAction::ActivateSkillSlot3;
        case Qt::Key_4:
            return GameAction::ActivateSkillSlot4;
        case Qt::Key_Escape:
        case Qt::Key_P:
            return GameAction::Pause;
        default:
            return GameAction::Count;
    }
}

void GameBoardWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->isAutoRepeat()) {
        return;
    }

    const GameAction action = keyToAction(event->key());
    if (action != GameAction::Count && m_engine) {
        m_engine->processInput(action, true);
    }

    QWidget::keyPressEvent(event);
}

void GameBoardWidget::keyReleaseEvent(QKeyEvent* event)
{
    if (event->isAutoRepeat()) return;

    const GameAction action = keyToAction(event->key());
    if (action != GameAction::Count && m_engine) {
        m_engine->processInput(action, false);
    }

    QWidget::keyReleaseEvent(event);
}
