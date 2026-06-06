#ifndef LINECLEARSKILL_H
#define LINECLEARSKILL_H

#include "skills/SkillBase.h"

class LineClearSkill : public SkillBase {
public:
    void onActivate(TetrisBoard& board, GameEngine& engine) override;

    QString name() const override { return QStringLiteral("直线清扫"); }
    QString description() const override { return QStringLiteral("清除当前最危险的一行，适合快速拆弹。"); }
    QColor themeColor() const override { return QColor(90, 240, 200); }
    QString typeId() const override { return QStringLiteral("line_clear"); }
    bool isInstant() const override { return true; }
    int energyCost() const override { return 60; }
};

#endif
