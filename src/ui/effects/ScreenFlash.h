#ifndef SCREENFLASH_H
#define SCREENFLASH_H

#include <QObject>
#include <QColor>
#include <QElapsedTimer>

class QPainter;
class QRect;

class ScreenFlash : public QObject {
    Q_OBJECT
public:
    explicit ScreenFlash(QObject* parent = nullptr);

    void flash(const QColor& color, int durationMs);
    void draw(QPainter& painter, const QRect& area);
    bool isFlashing() const;

private:
    QColor m_color;
    int m_duration = 0;
    QElapsedTimer m_timer;
};

#endif
