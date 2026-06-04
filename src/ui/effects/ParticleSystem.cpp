#include "ParticleSystem.h"
#include <QPainter>
#include <random>
#include <cmath>

ParticleSystem::ParticleSystem(QObject* parent)
    : QObject(parent)
{
}

void ParticleSystem::burst(const QPointF& center, const QColor& baseColor, int count)
{
    static std::mt19937 rng(42);
    static std::uniform_real_distribution<float> angleDist(0.0f, 6.28318f);
    static std::uniform_real_distribution<float> speedDist(1.0f, 5.0f);
    static std::uniform_real_distribution<float> sizeDist(2.0f, 6.0f);

    for (int i = 0; i < count; ++i) {
        Particle p;
        p.pos = center;
        float angle = angleDist(rng);
        float speed = speedDist(rng);
        p.vel = QPointF(std::cos(angle) * speed, std::sin(angle) * speed);
        p.color = baseColor.lighter(50 + (rng() % 100));
        p.size = sizeDist(rng);
        p.decay = 0.015f + (rng() % 100) * 0.0003f;
        p.life = 1.0f;
        m_particles.push_back(p);
    }
}

void ParticleSystem::update()
{
    for (auto& p : m_particles) {
        p.pos += p.vel;
        p.life -= p.decay;
        p.vel.setY(p.vel.y() + 0.05f); // gravity
    }

    // Remove dead particles
    m_particles.erase(
        std::remove_if(m_particles.begin(), m_particles.end(),
                       [](const Particle& p) { return p.life <= 0.0f; }),
        m_particles.end());
}

void ParticleSystem::draw(QPainter& painter)
{
    update();
    for (auto& p : m_particles) {
        painter.setOpacity(static_cast<double>(p.life));
        painter.setBrush(p.color);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(p.pos, p.size, p.size);
    }
    painter.setOpacity(1.0);
}
