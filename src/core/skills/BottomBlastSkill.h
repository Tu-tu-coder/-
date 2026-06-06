#ifndef BOTTOMBLASTSKILL_H
#define BOTTOMBLASTSKILL_H

#include "skills/SkillBase.h"

class BottomBlastSkill : public SkillBase {
public:
    void onActivate(TetrisBoard& board, GameEngine& engine) override;

    QString name() const override { return QStringLiteral("底部爆破"); }
    QString description() const override { return QStringLiteral("清除最底部 1 行，并让上方方块整体下落。"); }
    QColor themeColor() const override { return QColor(255, 170, 70); }
    QString typeId() const override { return QStringLiteral("bottom_blast"); }
    bool isInstant() const override { return true; }
    int energyCost() const override { return 45; }
};

#endif
