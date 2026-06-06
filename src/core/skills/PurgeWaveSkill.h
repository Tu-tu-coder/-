#ifndef PURGEWAVESKILL_H
#define PURGEWAVESKILL_H

#include "skills/SkillBase.h"

class PurgeWaveSkill : public SkillBase {
public:
    void onActivate(TetrisBoard& board, GameEngine& engine) override;

    QString name() const override { return QStringLiteral("净场脉冲"); }
    QString description() const override { return QStringLiteral("清除底部 4 行，代价极高，但能强行续命。"); }
    QColor themeColor() const override { return QColor(255, 90, 120); }
    QString typeId() const override { return QStringLiteral("purge_wave"); }
    bool isInstant() const override { return true; }
    int energyCost() const override { return 120; }
};

#endif
