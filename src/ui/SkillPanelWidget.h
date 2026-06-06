#ifndef SKILLPANELWIDGET_H
#define SKILLPANELWIDGET_H

#include <QFrame>
#include <QLabel>
#include <QWidget>

#include "../core/GameConfig.h"

class GameEngine;
class SkillManager;

class SkillPanelWidget : public QWidget {
    Q_OBJECT
public:
    explicit SkillPanelWidget(SkillManager* skillMgr, GameEngine* engine, QWidget* parent = nullptr);

public slots:
    void updateSkill();

private:
    void setSlotStyle(int index, const QString& skillId);

    SkillManager* m_skillMgr = nullptr;
    GameEngine* m_engine = nullptr;
    QLabel* m_titleLabel = nullptr;
    QLabel* m_stateLabel = nullptr;
    QFrame* m_slotFrames[GameConfig::kMaxCarrySlots] = {};
    QLabel* m_slotLabels[GameConfig::kMaxCarrySlots] = {};
};

#endif
