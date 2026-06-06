#include "SkillPanelWidget.h"

#include "../core/GameConfig.h"
#include "../core/GameEngine.h"
#include "../core/SkillManager.h"

#include <QVBoxLayout>

SkillPanelWidget::SkillPanelWidget(SkillManager* skillMgr, GameEngine* engine, QWidget* parent)
    : QWidget(parent), m_skillMgr(skillMgr), m_engine(engine)
{
    auto* layout = new QVBoxLayout(this);
    layout->setSpacing(6);
    layout->setContentsMargins(8, 8, 8, 8);

    m_titleLabel = new QLabel(QStringLiteral("技能槽"));
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(10);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setStyleSheet("color: #8888BB;");
    layout->addWidget(m_titleLabel);

    m_stateLabel = new QLabel;
    m_stateLabel->setWordWrap(true);
    m_stateLabel->setStyleSheet("color: #9AAAE0; font-size: 10px;");
    layout->addWidget(m_stateLabel);

    for (int i = 0; i < GameConfig::kMaxCarrySlots; ++i) {
        m_slotFrames[i] = new QFrame;
        m_slotFrames[i]->setFrameShape(QFrame::StyledPanel);
        auto* slotLayout = new QVBoxLayout(m_slotFrames[i]);
        slotLayout->setContentsMargins(8, 6, 8, 6);
        slotLayout->setSpacing(2);

        m_slotLabels[i] = new QLabel;
        m_slotLabels[i]->setWordWrap(true);
        slotLayout->addWidget(m_slotLabels[i]);
        layout->addWidget(m_slotFrames[i]);
    }

    setMinimumWidth(180);
    setStyleSheet("background-color: #1E1E32; border-radius: 5px;");

    if (m_skillMgr) {
        connect(m_skillMgr, &SkillManager::loadoutChanged, this, &SkillPanelWidget::updateSkill);
        connect(m_skillMgr, &SkillManager::energyChanged, this, [this](int, int) { updateSkill(); });
        connect(m_skillMgr, &SkillManager::skillActivated, this, [this](const QString&) { updateSkill(); });
        connect(m_skillMgr, &SkillManager::skillDeactivated, this, [this](const QString&) { updateSkill(); });
        connect(m_skillMgr, &SkillManager::defaultSlotChanged, this, [this](int) { updateSkill(); });
    }
    if (m_engine) {
        connect(m_engine, &GameEngine::gameModeChanged, this, [this](GameMode) { updateSkill(); });
    }

    updateSkill();
}

void SkillPanelWidget::updateSkill()
{
    if (!m_skillMgr || !m_engine) return;

    if (!m_engine->isSkillsEnabled()) {
        m_stateLabel->setText(QStringLiteral("Classic Mode: Skills Disabled"));
        for (int i = 0; i < GameConfig::kMaxCarrySlots; ++i) {
            m_slotLabels[i]->setText(QStringLiteral("槽位 %1\n未启用").arg(i + 1));
            m_slotFrames[i]->setStyleSheet("background: #23233A; border: 1px solid #38385A; border-radius: 6px;");
        }
        return;
    }

    m_stateLabel->setText(QStringLiteral("数字键 1-%1 直接释放，V/E 释放第 1 技能。")
        .arg(GameConfig::kMaxCarrySlots));

    const QStringList loadout = m_skillMgr->loadoutSkills();
    for (int i = 0; i < GameConfig::kMaxCarrySlots; ++i) {
        if (i >= loadout.size()) {
            m_slotLabels[i]->setText(QStringLiteral("%1号槽\n未解锁").arg(i + 1));
            m_slotFrames[i]->setStyleSheet("background: #23233A; border: 1px dashed #4A4A70; border-radius: 6px;");
            continue;
        }

        const QString skillId = loadout[i];
        const QString status = m_skillMgr->activeSkillId() == skillId
            ? QStringLiteral("激活中")
            : (m_skillMgr->canActivateSlot(i) ? QStringLiteral("可释放") : QStringLiteral("能量不足"));
        m_slotLabels[i]->setText(QStringLiteral("[%1] %2\n消耗：%3  |  %4\n%5")
            .arg(i + 1)
            .arg(m_skillMgr->skillName(skillId))
            .arg(m_skillMgr->skillEnergyCost(skillId))
            .arg(status)
            .arg(m_skillMgr->skillDescription(skillId)));
        setSlotStyle(i, skillId);
    }
}

void SkillPanelWidget::setSlotStyle(int index, const QString& skillId)
{
    QColor border = m_skillMgr->skillColor(skillId);
    QString borderHex = border.name();
    QString background = (m_skillMgr->defaultSlot() == index)
        ? QStringLiteral("#2C2446")
        : QStringLiteral("#23233A");
    QString weight = m_skillMgr->activeSkillId() == skillId ? QStringLiteral("2px") : QStringLiteral("1px");
    m_slotFrames[index]->setStyleSheet(QString(
        "background: %1; border: %2 solid %3; border-radius: 6px;")
        .arg(background, weight, borderHex));
}
