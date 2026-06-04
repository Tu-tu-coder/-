#ifndef SCREENSHAKE_H
#define SCREENSHAKE_H

#include <QObject>
#include <QElapsedTimer>
#include <utility>

class ScreenShake : public QObject {
    Q_OBJECT
public:
    explicit ScreenShake(QObject* parent = nullptr);

    void shake(float intensity, int durationMs);
    std::pair<float, float> currentOffset() const;
    bool isShaking() const;

private:
    float m_intensity = 0.0f;
    int m_duration = 0;
    QElapsedTimer m_timer;
};

#endif
