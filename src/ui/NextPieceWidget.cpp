#include "NextPieceWidget.h"
#include "../core/GameEngine.h"
#include "../core/GameConfig.h"
#include "../core/Tetromino.h"
#include <QPainter>

NextPieceWidget::NextPieceWidget(GameEngine* engine, QWidget* parent)
    : QWidget(parent), m_engine(engine)
{
    setMinimumSize(120, 100);
    setMaximumHeight(120);
    if (m_engine) {
        connect(m_engine, &GameEngine::pieceSpawned,
                this, QOverload<>::of(&QWidget::update));
    }
}

void NextPieceWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    // Background
    p.fillRect(rect(), QColor(30, 30, 50));
    p.setPen(QPen(QColor(80, 80, 120), 1));
    p.drawRect(rect().adjusted(0, 0, -1, -1));

    // Title
    QFont titleFont = p.font();
    titleFont.setPointSize(10);
    titleFont.setBold(true);
    p.setFont(titleFont);
    p.setPen(QColor(150, 150, 200));
    p.drawText(QRect(0, 5, width(), 20), Qt::AlignHCenter, "Next");

    if (!m_engine) return;
    if (m_engine->state() == GameState::Idle) return;

    auto& piece = m_engine->nextPiece();
    auto cells = piece.occupiedCells();
    QColor color = piece.color();

    int cellSz = 18;
    int totalW = 4 * cellSz;
    int totalH = 4 * cellSz;
    int offsetX = (width() - totalW) / 2;
    int offsetY = (height() - totalH + 15) / 2; // offset for title

    for (auto& [r, c] : cells) {
        int x = offsetX + c * cellSz;
        int y = offsetY + r * cellSz;
        QRect cr(x + 1, y + 1, cellSz - 2, cellSz - 2);

        p.setBrush(color);
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(cr, 2, 2);

        p.setPen(QPen(color.lighter(150), 1));
        p.drawLine(cr.topLeft(), cr.topRight());
        p.drawLine(cr.topLeft(), cr.bottomLeft());
        p.setPen(QPen(color.darker(150), 1));
        p.drawLine(cr.bottomLeft(), cr.bottomRight());
        p.drawLine(cr.topRight(), cr.bottomRight());
    }
}
