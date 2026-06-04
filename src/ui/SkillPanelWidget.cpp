#include "SkillPanelWidget.h"
#include "../core/SkillManager.h"
#include "../core/skills/SkillFactory.h"
#include <QPainter>
#include <QVBoxLayout>

SkillPanelWidget::SkillPanelWidget(SkillManager* skillMgr, QWidget* parent)
    : QWidget(parent), m_skillMgr(skillMgr)
{
    auto* layout = new QVBoxLayout(this);
    layout->setSpacing(4);
    layout->setContentsMargins(8, 8, 8, 8);

    auto* title = new QLabel("Equipped Skill");
    QFont tf = title->font();
    tf.setPointSize(9);
    tf.setBold(true);
    title->setFont(tf);
    title->setStyleSheet("color: #8888BB;");
    layout->addWidget(title);

    m_nameLabel = new QLabel("None");
    QFont nf = m_nameLabel->font();
    nf.setPointSize(13);
    nf.setBold(true);
    m_nameLabel->setFont(nf);
    m_nameLabel->setStyleSheet("color: #FFD700;");
    layout->addWidget(m_nameLabel);

    m_descLabel = new QLabel("");
    m_descLabel->setWordWrap(true);
    m_descLabel->setStyleSheet("color: #AAAACC; font-size: 10px;");
    m_descLabel->setMaximumHeight(40);
    layout->addWidget(m_descLabel);

    m_keyHintLabel = new QLabel("Press [V] to activate");
    m_keyHintLabel->setStyleSheet("color: #777799; font-size: 9px;");
    layout->addWidget(m_keyHintLabel);

    setStyleSheet("background-color: #1E1E32; border-radius: 5px;");
    setMinimumWidth(140);

    if (m_skillMgr) {
        connect(m_skillMgr, &SkillManager::skillEquipped,
                this, [this](const QString&) { updateSkill(); });
        connect(m_skillMgr, &SkillManager::skillActivated,
                this, [this](const QString&) { updateSkill(); });
        connect(m_skillMgr, &SkillManager::skillDeactivated,
                this, [this](const QString&) { updateSkill(); });
        updateSkill();
    }
}

void SkillPanelWidget::updateSkill()
{
    if (!m_skillMgr) return;
    QString skillId = m_skillMgr->equippedSkillId();
    if (skillId.isEmpty()) {
        m_nameLabel->setText("None");
        m_descLabel->setText("Select a skill before starting");
        m_keyHintLabel->setText("");
        return;
    }
    m_nameLabel->setText(m_skillMgr->skillName(skillId));
    m_descLabel->setText(m_skillMgr->skillDescription(skillId));

    if (m_skillMgr->isSkillActive()) {
        m_keyHintLabel->setText("⚡ ACTIVE ⚡");
        m_keyHintLabel->setStyleSheet("color: #00DDFF; font-size: 9px; font-weight: bold;");
    } else if (m_skillMgr->isEnergyFull()) {
        m_keyHintLabel->setText("READY! Press [V]");
        m_keyHintLabel->setStyleSheet("color: #FFD700; font-size: 9px; font-weight: bold;");
    } else {
        m_keyHintLabel->setText(QString("Cost: %1 energy").arg(m_skillMgr->skillEnergyCost(skillId)));
        m_keyHintLabel->setStyleSheet("color: #777799; font-size: 9px;");
    }
}

void SkillPanelWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    // Draw skill color indicator
    if (m_skillMgr && !m_skillMgr->equippedSkillId().isEmpty()) {
        QColor color = m_skillMgr->skillColor(m_skillMgr->equippedSkillId());
        p.setBrush(color);
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(10, 38, 8, 8, 2, 2);
    }
}
