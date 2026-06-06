#include "EnergyBarWidget.h"

#include "../core/SkillManager.h"

#include <QPainter>
#include <QRadialGradient>

EnergyBarWidget::EnergyBarWidget(SkillManager* skillMgr, QWidget* parent)
    : QWidget(parent), m_skillMgr(skillMgr)
{
    setMinimumHeight(30);
    setMaximumHeight(40);

    m_glowAnim = new QPropertyAnimation(this, "glowAlpha", this);
    m_glowAnim->setDuration(800);
    m_glowAnim->setStartValue(0.0f);
    m_glowAnim->setEndValue(1.0f);
    m_glowAnim->setLoopCount(-1);

    if (m_skillMgr) {
        connect(m_skillMgr, &SkillManager::energyChanged, this, [this](int current, int max) {
            m_energy = current;
            m_maxEnergy = max;
            if (current >= max) {
                if (m_glowAnim->state() != QAbstractAnimation::Running) {
                    m_glowAnim->start();
                }
            } else {
                m_glowAnim->stop();
                m_glowAlpha = 0.0f;
            }
            update();
        });
    }
}

void EnergyBarWidget::setGlowAlpha(float alpha)
{
    m_glowAlpha = alpha;
    update();
}

void EnergyBarWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    const int barWidth = width() - 4;
    const int barHeight = height() - 8;
    const int barX = 2;
    const int barY = 4;

    p.fillRect(rect(), QColor(25, 25, 45));
    p.setPen(QPen(QColor(60, 60, 90), 1));
    p.drawRoundedRect(barX, barY, barWidth, barHeight, 6, 6);

    if (m_energy > 0) {
        const float fillRatio = static_cast<float>(m_energy) / m_maxEnergy;
        const int fillW = static_cast<int>((barWidth - 4) * fillRatio);
        const int fillX = barX + 2;
        const int fillY = barY + 2;
        const int fillH = barHeight - 4;

        QLinearGradient grad(fillX, 0, fillX + fillW, 0);
        grad.setColorAt(0.0, QColor(0, 180, 255));
        grad.setColorAt(0.5, QColor(0, 220, 255));
        grad.setColorAt(1.0, QColor(0, 255, 200));
        p.setBrush(grad);
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(fillX, fillY, fillW, fillH, 3, 3);

        if (m_glowAlpha > 0.01f) {
            QRadialGradient glow(fillX + fillW - 10, fillY + fillH / 2, 30);
            glow.setColorAt(0.0, QColor(100, 220, 255, static_cast<int>(150 * m_glowAlpha)));
            glow.setColorAt(1.0, QColor(0, 200, 255, 0));
            p.setBrush(glow);
            p.drawRect(rect());
        }
    }

    p.setPen(QColor(200, 220, 255));
    QFont f = p.font();
    f.setPointSize(9);
    p.setFont(f);
    QString text = QStringLiteral("Energy %1 / %2").arg(m_energy).arg(m_maxEnergy);
    if (m_energy >= m_maxEnergy) {
        text += QStringLiteral("  FULL");
    }
    p.drawText(QRect(barX, barY, barWidth, barHeight), Qt::AlignCenter, text);
}
