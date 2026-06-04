#include "MenuWidget.h"
#include "../core/SkillManager.h"
#include <QApplication>

MenuWidget::MenuWidget(SkillManager* skillMgr, QWidget* parent)
    : QWidget(parent), m_skillMgr(skillMgr)
{
    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);

    // Title
    auto* title = new QLabel("俄罗斯方块：技能大作战");
    QFont titleFont;
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setStyleSheet("color: #FFD700;");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    auto* subtitle = new QLabel("Tetris: Skill Battle");
    QFont subFont;
    subFont.setPointSize(12);
    subtitle->setFont(subFont);
    subtitle->setStyleSheet("color: #8888BB;");
    subtitle->setAlignment(Qt::AlignCenter);
    layout->addWidget(subtitle);

    layout->addSpacing(30);

    // Skill selection (for challenge mode)
    auto* skillLabel = new QLabel("Select Skill (Challenge Mode):");
    skillLabel->setStyleSheet("color: #AAAACC; font-size: 11px;");
    skillLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(skillLabel);

    m_skillCombo = new QComboBox;
    m_skillCombo->setMinimumWidth(200);
    m_skillCombo->setStyleSheet(
        "QComboBox { background: #2A2A45; color: white; border: 1px solid #555; "
        "padding: 6px; border-radius: 4px; }"
        "QComboBox::drop-down { border: none; }"
    );

    // Populate skill list
    if (m_skillMgr) {
        auto skills = m_skillMgr->availableSkills();
        for (auto& id : skills) {
            m_skillCombo->addItem(m_skillMgr->skillName(id), id);
        }
    }
    layout->addWidget(m_skillCombo, 0, Qt::AlignCenter);

    layout->addSpacing(20);

    // Challenge mode button
    m_challengeBtn = new QPushButton("⚔ 无限挑战模式 (Infinite Challenge)");
    m_challengeBtn->setMinimumSize(300, 50);
    m_challengeBtn->setStyleSheet(
        "QPushButton { background: #3A1A5A; color: #FFD700; border: 2px solid #6A3AAA; "
        "border-radius: 8px; font-size: 14px; font-weight: bold; padding: 10px; }"
        "QPushButton:hover { background: #5A2A8A; }"
    );
    layout->addWidget(m_challengeBtn, 0, Qt::AlignCenter);

    // Classic mode button
    m_classicBtn = new QPushButton("□ 经典模式 (Classic)");
    m_classicBtn->setMinimumSize(300, 45);
    m_classicBtn->setStyleSheet(
        "QPushButton { background: #1A3A5A; color: #CCDDFF; border: 2px solid #3A6AAA; "
        "border-radius: 8px; font-size: 13px; padding: 8px; }"
        "QPushButton:hover { background: #2A5A8A; }"
    );
    layout->addWidget(m_classicBtn, 0, Qt::AlignCenter);

    layout->addSpacing(20);

    // Controls info
    auto* controls = new QLabel(
        "← → 移动  |  ↑ 旋转  |  ↓ 软降  |  Space 硬降  |  V 技能  |  Esc 暂停");
    controls->setStyleSheet("color: #666688; font-size: 10px;");
    controls->setAlignment(Qt::AlignCenter);
    layout->addWidget(controls);

    // Connections
    connect(m_challengeBtn, &QPushButton::clicked, this, &MenuWidget::onStartChallenge);
    connect(m_classicBtn, &QPushButton::clicked, this, &MenuWidget::onStartClassic);

    setStyleSheet("background-color: #12122A;");
}

void MenuWidget::onStartClassic()
{
    emit startGame(GameMode::Classic, "");
}

void MenuWidget::onStartChallenge()
{
    QString skillId = m_skillCombo->currentData().toString();
    emit startGame(GameMode::InfiniteChallenge, skillId);
}
