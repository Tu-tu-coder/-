#ifndef ENERGYBARWIDGET_H
#define ENERGYBARWIDGET_H

#include <QPropertyAnimation>
#include <QWidget>

#include "../core/GameConfig.h"

class SkillManager;

class EnergyBarWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(float glowAlpha READ glowAlpha WRITE setGlowAlpha)
public:
    explicit EnergyBarWidget(SkillManager* skillMgr, QWidget* parent = nullptr);

    float glowAlpha() const { return m_glowAlpha; }
    void setGlowAlpha(float alpha);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    SkillManager* m_skillMgr = nullptr;
    float m_glowAlpha = 0.0f;
    QPropertyAnimation* m_glowAnim = nullptr;
    int m_energy = 0;
    int m_maxEnergy = GameConfig::kEnergyMax;
};

#endif
