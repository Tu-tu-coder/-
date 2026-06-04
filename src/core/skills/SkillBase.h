#ifndef SKILLBASE_H
#define SKILLBASE_H

#include <QString>
#include <QColor>
#include <QJsonObject>

class TetrisBoard;
class GameEngine;

// Abstract base class for all skills
// To add a new skill: inherit from this, override onActivate (required),
// onTick/onDeactivate (optional), and use REGISTER_SKILL macro.
class SkillBase {
public:
    virtual ~SkillBase() = default;

    // === Lifecycle ===
    virtual void onActivate(TetrisBoard& board, GameEngine& engine) = 0;
    virtual void onTick(TetrisBoard& board, GameEngine& engine, qint64 deltaMs) {}
    virtual void onDeactivate(TetrisBoard& board, GameEngine& engine) {}

    // === Metadata ===
    virtual QString name() const = 0;
    virtual QString description() const = 0;
    virtual QColor themeColor() const { return QColor(100, 100, 200); }

    // === Timing ===
    virtual bool isInstant() const { return false; }
    virtual qint64 duration() const { return 0; }     // ms, 0 = instant
    virtual int energyCost() const { return 100; }

    // === Effect modifiers ===
    virtual float gravityMultiplier() const { return 1.0f; }

    // === Persistence ===
    virtual QString typeId() const = 0;
    virtual QJsonObject toJson() const;
    virtual void fromJson(const QJsonObject& json);

    // === Runtime state ===
    qint64 remainingMs() const { return m_remainingMs; }
    void setRemainingMs(qint64 ms) { m_remainingMs = ms; }
    bool isExpired() const { return !isInstant() && m_remainingMs <= 0; }

protected:
    qint64 m_remainingMs = 0;
};

#endif // SKILLBASE_H
