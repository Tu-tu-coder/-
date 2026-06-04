#ifndef GAMEBOARDWIDGET_H
#define GAMEBOARDWIDGET_H

#include <QWidget>
#include <QElapsedTimer>
#include "../core/GameEngine.h"

class ScreenFlash;
class ScreenShake;
class ParticleSystem;

class GameBoardWidget : public QWidget {
    Q_OBJECT
public:
    explicit GameBoardWidget(GameEngine* engine, QWidget* parent = nullptr);

    void setEngine(GameEngine* engine);

    // Effects access
    ScreenFlash* screenFlash() const { return m_flash; }
    ScreenShake* screenShake() const { return m_shake; }
    ParticleSystem* particles() const { return m_particles; }

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

private:
    GameAction keyToAction(int key) const;

    void drawBoard(QPainter& p);
    void drawCell(QPainter& p, int col, int row, const QColor& color, float alpha = 1.0f);
    void drawGhost(QPainter& p);
    void drawActivePiece(QPainter& p);
    void drawGrid(QPainter& p);
    void drawEffects(QPainter& p);

    int cellSize() const;
    int boardPixelWidth() const;
    int boardPixelHeight() const;

    GameEngine* m_engine = nullptr;
    ScreenFlash* m_flash = nullptr;
    ScreenShake* m_shake = nullptr;
    ParticleSystem* m_particles = nullptr;

    // Board offset for centering
    int m_offsetX = 0;
    int m_offsetY = 0;

    // Animation
    QElapsedTimer m_lineClearTimer;
    std::vector<int> m_clearingRows; // rows currently flashing
    static constexpr int kClearFlashMs = 300;
};

#endif // GAMEBOARDWIDGET_H
