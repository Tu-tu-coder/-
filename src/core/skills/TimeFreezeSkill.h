#ifndef TIMEFREEZESKILL_H
#define TIMEFREEZESKILL_H

#include "GameConfig.h"
#include "skills/SkillBase.h"

class TimeFreezeSkill : public SkillBase {
public:
    void onActivate(TetrisBoard& board, GameEngine& engine) override;
    void onTick(TetrisBoard& board, GameEngine& engine, qint64 deltaMs) override;
    void onDeactivate(TetrisBoard& board, GameEngine& engine) override;

    QString name() const override { return QStringLiteral("时间凝滞"); }
    QString description() const override { return QStringLiteral("显著降低当前下落速度，适合紧急救场。"); }
    QColor themeColor() const override { return QColor(60, 160, 255); }
    QString typeId() const override { return QStringLiteral("time_freeze"); }
    bool isInstant() const override { return false; }
    qint64 duration() const override { return GameConfig::kTimeFreezeDuration; }
    int energyCost() const override { return 35; }
    float gravityMultiplier() const override { return GameConfig::kTimeFreezeSlowFactor; }
};

#endif // TIMEFREEZESKILL_H
