#include "MenuWidget.h"

#include "../core/GameConfig.h"
#include "../core/SkillManager.h"

MenuWidget::MenuWidget(SkillManager* skillMgr, QWidget* parent)
    : QWidget(parent), m_skillMgr(skillMgr)
{
    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);
    layout->setSpacing(14);
    layout->setContentsMargins(40, 24, 40, 24);

    auto* title = new QLabel(QStringLiteral("俄罗斯方块：技能大作战"));
    QFont titleFont;
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setStyleSheet("color: #FFD700;");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    auto* subtitle = new QLabel(QStringLiteral("Tetris: Skill Battle"));
    QFont subFont;
    subFont.setPointSize(12);
    subtitle->setFont(subFont);
    subtitle->setStyleSheet("color: #8888BB;");
    subtitle->setAlignment(Qt::AlignCenter);
    layout->addWidget(subtitle);

    m_progressLabel = new QLabel;
    m_progressLabel->setAlignment(Qt::AlignCenter);
    m_progressLabel->setStyleSheet("color: #CCDDFF; font-size: 12px;");
    layout->addWidget(m_progressLabel);

    m_loadoutTitleLabel = new QLabel(QStringLiteral("挑战模式携带技能"));
    m_loadoutTitleLabel->setStyleSheet("color: #FFD700; font-size: 13px; font-weight: bold;");
    layout->addWidget(m_loadoutTitleLabel);

    m_loadoutHintLabel = new QLabel;
    m_loadoutHintLabel->setWordWrap(true);
    m_loadoutHintLabel->setStyleSheet("color: #AAAACC; font-size: 11px;");
    layout->addWidget(m_loadoutHintLabel);

    m_loadoutContainer = new QWidget;
    m_loadoutLayout = new QVBoxLayout(m_loadoutContainer);
    m_loadoutLayout->setSpacing(8);
    m_loadoutLayout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_loadoutContainer);

    m_challengeBtn = new QPushButton(QStringLiteral("⚔ 无限挑战模式 (Infinite Challenge)"));
    m_challengeBtn->setMinimumSize(340, 52);
    m_challengeBtn->setStyleSheet(
        "QPushButton { background: #3A1A5A; color: #FFD700; border: 2px solid #6A3AAA; "
        "border-radius: 8px; font-size: 14px; font-weight: bold; padding: 10px; }"
        "QPushButton:hover:enabled { background: #5A2A8A; }"
        "QPushButton:disabled { background: #26263A; color: #8A8AA8; border: 2px solid #44445A; }");
    layout->addWidget(m_challengeBtn, 0, Qt::AlignCenter);

    m_classicBtn = new QPushButton(QStringLiteral("■ 经典模式 (Classic)"));
    m_classicBtn->setMinimumSize(340, 46);
    m_classicBtn->setStyleSheet(
        "QPushButton { background: #1A3A5A; color: #CCDDFF; border: 2px solid #3A6AAA; "
        "border-radius: 8px; font-size: 13px; padding: 8px; }"
        "QPushButton:hover { background: #2A5A8A; }");
    layout->addWidget(m_classicBtn, 0, Qt::AlignCenter);

    m_leaderboardLabel = new QLabel;
    m_leaderboardLabel->setAlignment(Qt::AlignCenter);
    m_leaderboardLabel->setStyleSheet(
        "background: #1C1C34; color: #BFD4FF; border: 1px solid #303060; "
        "border-radius: 8px; padding: 10px; font-size: 11px;");
    m_leaderboardLabel->setMinimumWidth(360);
    layout->addWidget(m_leaderboardLabel);

    auto* controls = new QLabel(
        QStringLiteral("←/A 移动  |  ↑/W 旋转  |  Z 逆旋  |  ↓/S 软降  |  Space 硬降\n"
                       "1-4 释放技能  |  V/E 第1技能  |  Esc/P 暂停"));
    controls->setStyleSheet("color: #777799; font-size: 10px;");
    controls->setAlignment(Qt::AlignCenter);
    layout->addWidget(controls);

    connect(m_challengeBtn, &QPushButton::clicked, this, [this]() {
        emit startGame(GameMode::InfiniteChallenge, m_selectedLoadoutSkillIds);
    });
    connect(m_classicBtn, &QPushButton::clicked, this, [this]() {
        emit startGame(GameMode::Classic, {});
    });

    setStyleSheet("background-color: #12122A;");
}

void MenuWidget::setProgressSnapshot(const ProgressSnapshot& snapshot)
{
    m_snapshot = snapshot;
    m_selectedLoadoutSkillIds = snapshot.selectedLoadoutSkillIds;

    m_progressLabel->setText(QStringLiteral("最高分：%1  |  已解锁技能：%2 / 6  |  挑战模式槽位：%3 / %4")
        .arg(snapshot.highScore)
        .arg(snapshot.unlockedSkillIds.size())
        .arg(snapshot.maxCarrySlots)
        .arg(GameConfig::kMaxCarrySlots));

    QStringList lines;
    lines << QStringLiteral("本地排行榜 Top %1").arg(GameConfig::kLeaderboardSize);
    if (snapshot.topScores.isEmpty()) {
        lines << QStringLiteral("暂无记录，先打出第一局吧。");
    } else {
        for (int i = 0; i < snapshot.topScores.size(); ++i) {
            const ScoreEntry& entry = snapshot.topScores[i];
            lines << QStringLiteral("%1. %2 分  |  Lv.%3  |  %4 行  |  %5")
                .arg(i + 1)
                .arg(entry.score)
                .arg(entry.level)
                .arg(entry.lines)
                .arg(entry.mode);
        }
    }
    m_leaderboardLabel->setText(lines.join('\n'));

    rebuildSkillButtons();
    updateLoadoutUI();
}

void MenuWidget::rebuildSkillButtons()
{
    while (QLayoutItem* item = m_loadoutLayout->takeAt(0)) {
        delete item->widget();
        delete item;
    }
    m_skillButtons.clear();

    for (const QString& skillId : m_snapshot.unlockedSkillIds) {
        auto* button = new QPushButton(skillButtonText(skillId));
        button->setCheckable(true);
        button->setMinimumHeight(56);
        button->setStyleSheet(
            "QPushButton { background: #20203A; color: #D8E4FF; border: 1px solid #3A3A60; "
            "border-radius: 8px; text-align: left; padding: 8px; }"
            "QPushButton:checked { background: #2D2A4A; border: 2px solid #68B7FF; }");
        connect(button, &QPushButton::clicked, this, [this, skillId]() {
            onSkillButtonClicked(skillId);
        });
        m_loadoutLayout->addWidget(button);
        m_skillButtons.insert(skillId, button);
    }
}

void MenuWidget::updateLoadoutUI()
{
    const int selectedCount = m_selectedLoadoutSkillIds.size();
    const int requiredCount = m_snapshot.maxCarrySlots;
    m_loadoutTitleLabel->setText(QStringLiteral("挑战模式携带技能（需恰好选择 %1 个）").arg(requiredCount));

    if (m_snapshot.unlockedSkillIds.isEmpty()) {
        m_loadoutHintLabel->setText(QStringLiteral("当前还没有可用技能。"));
    } else if (selectedCount == requiredCount) {
        m_loadoutHintLabel->setText(QStringLiteral("已选满：局内 1-%1 键顺序与这里的选择顺序一致。").arg(requiredCount));
    } else {
        m_loadoutHintLabel->setText(QStringLiteral("当前已选择 %1 / %2 个技能。必须选满后才能开始挑战模式。")
            .arg(selectedCount)
            .arg(requiredCount));
    }

    for (const QString& skillId : m_snapshot.unlockedSkillIds) {
        QPushButton* button = m_skillButtons.value(skillId, nullptr);
        if (!button) continue;
        button->setText(skillButtonText(skillId));
        button->setChecked(m_selectedLoadoutSkillIds.contains(skillId));
    }

    m_challengeBtn->setEnabled(selectedCount == requiredCount);
}

void MenuWidget::onSkillButtonClicked(const QString& skillId)
{
    const int currentIndex = m_selectedLoadoutSkillIds.indexOf(skillId);
    if (currentIndex >= 0) {
        m_selectedLoadoutSkillIds.removeAt(currentIndex);
    } else if (m_selectedLoadoutSkillIds.size() < m_snapshot.maxCarrySlots) {
        m_selectedLoadoutSkillIds.push_back(skillId);
    }
    updateLoadoutUI();
}

QString MenuWidget::skillButtonText(const QString& skillId) const
{
    const int index = m_selectedLoadoutSkillIds.indexOf(skillId);
    const QString orderLabel = index >= 0
        ? QStringLiteral("[%1]").arg(index + 1)
        : QStringLiteral("[未选]");
    return QStringLiteral("%1 %2  |  消耗 %3\n%4")
        .arg(orderLabel)
        .arg(m_skillMgr ? m_skillMgr->skillName(skillId) : skillId)
        .arg(m_skillMgr ? m_skillMgr->skillEnergyCost(skillId) : 0)
        .arg(m_skillMgr ? m_skillMgr->skillDescription(skillId) : QString());
}
