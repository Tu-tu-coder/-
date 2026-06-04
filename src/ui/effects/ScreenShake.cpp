#include "ScreenShake.h"
#include <random>
#include <cmath>

ScreenShake::ScreenShake(QObject* parent)
    : QObject(parent)
{
}

void ScreenShake::shake(float intensity, int durationMs)
{
    m_intensity = intensity;
    m_duration = durationMs;
    m_timer.start();
}

std::pair<float, float> ScreenShake::currentOffset() const
{
    if (!isShaking()) return {0.0f, 0.0f};

    qint64 elapsed = m_timer.elapsed();
    float decay = 1.0f - static_cast<float>(elapsed) / m_duration;
    float currentIntensity = m_intensity * decay;

    // Simple deterministic offset based on time for consistent rendering
    float phase = elapsed * 0.03f;
    float dx = std::sin(phase * 7.3f) * currentIntensity;
    float dy = std::cos(phase * 5.7f) * currentIntensity;

    return {dx, dy};
}

bool ScreenShake::isShaking() const
{
    return m_timer.isValid() && m_timer.elapsed() < m_duration;
}
