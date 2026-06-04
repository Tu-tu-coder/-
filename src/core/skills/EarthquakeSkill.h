#ifndef EARTHQUAKESKILL_H
#define EARTHQUAKESKILL_H

#include "skills/SkillBase.h"

class EarthquakeSkill : public SkillBase {
public:
    void onActivate(TetrisBoard& board, GameEngine& engine) override;

    QString name() const override { return QStringLiteral("大地震"); }
    QString description() const override {
        return QStringLiteral("消除最底部的两行（即使未满），上方方块整体下落");
    }
    QColor themeColor() const override { return QColor(180, 100, 40); }
    QString typeId() const override { return QStringLiteral("earthquake"); }

    bool isInstant() const override { return true; }
    int energyCost() const override { return 100; }
};

#endif // EARTHQUAKESKILL_H
