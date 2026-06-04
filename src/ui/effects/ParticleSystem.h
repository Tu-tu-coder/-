#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <QObject>
#include <QColor>
#include <QPointF>
#include <QElapsedTimer>
#include <vector>

class QPainter;

struct Particle {
    QPointF pos;
    QPointF vel;
    QColor color;
    float life = 1.0f;    // 1.0 = full, 0.0 = dead
    float decay = 0.02f;  // per frame
    float size = 4.0f;
};

class ParticleSystem : public QObject {
    Q_OBJECT
public:
    explicit ParticleSystem(QObject* parent = nullptr);

    void burst(const QPointF& center, const QColor& baseColor, int count);
    void update();
    void draw(QPainter& painter);

private:
    std::vector<Particle> m_particles;
};

#endif
