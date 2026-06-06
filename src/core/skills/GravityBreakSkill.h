#ifndef GRAVITYBREAKSKILL_H
#define GRAVITYBREAKSKILL_H

#include "GameConfig.h"
#include "skills/SkillBase.h"

class GravityBreakSkill : public SkillBase {
public:
    void onActivate(TetrisBoard& board, GameEngine& engine) override;
    void onTick(TetrisBoard& board, GameEngine& engine, qint64 deltaMs) override;
    void onDeactivate(TetrisBoard& board, GameEngine& engine) override;

    QString name() const override { return QStringLiteral("失重领域"); }
    QString description() const override { return QStringLiteral("短时间内极大降低下落速度，争取重整棋形。"); }
    QColor themeColor() const override { return QColor(175, 120, 255); }
    QString typeId() const override { return QStringLiteral("gravity_break"); }
    bool isInstant() const override { return false; }
    qint64 duration() const override { return GameConfig::kGravityBreakDuration; }
    int energyCost() const override { return 95; }
    float gravityMultiplier() const override { return GameConfig::kGravityBreakSlowFactor; }
};

#endif
