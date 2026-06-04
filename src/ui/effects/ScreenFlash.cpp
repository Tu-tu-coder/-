#include "ScreenFlash.h"
#include <QPainter>

ScreenFlash::ScreenFlash(QObject* parent)
    : QObject(parent)
{
}

void ScreenFlash::flash(const QColor& color, int durationMs)
{
    m_color = color;
    m_duration = durationMs;
    m_timer.start();
}

void ScreenFlash::draw(QPainter& painter, const QRect& area)
{
    if (!isFlashing()) return;

    qint64 elapsed = m_timer.elapsed();
    if (elapsed >= m_duration) return;

    float progress = static_cast<float>(elapsed) / m_duration;
    int alpha = static_cast<int>(m_color.alpha() * (1.0f - progress));
    QColor fadeColor(m_color.red(), m_color.green(), m_color.blue(), alpha);

    painter.fillRect(area, fadeColor);
}

bool ScreenFlash::isFlashing() const
{
    return m_timer.isValid() && m_timer.elapsed() < m_duration;
}
