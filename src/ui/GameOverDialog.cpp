#include "GameOverDialog.h"

#include "../core/GameConfig.h"
#include "../core/GameEngine.h"
#include "../core/SkillManager.h"

#include <QPushButton>
#include <QVBoxLayout>

GameOverDialog::GameOverDialog(GameEngine* engine, const ProgressUpdateResult& progressResult, QWidget* parent)
    : QDialog(parent), m_engine(engine), m_progressResult(progressResult)
{
    setWindowTitle(QStringLiteral("游戏结束"));
    setFixedSize(380, 420);
    setStyleSheet("background-color: #1A1A35; color: white;");

    auto* layout = new QVBoxLayout(this);
    layout->setSpacing(12);
    layout->setAlignment(Qt::AlignCenter);

    auto* title = new QLabel(QStringLiteral("游戏结束"));
    QFont titleFont;
    titleFont.setPointSize(22);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setStyleSheet("color: #FF6B6B;");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    m_scoreLabel = new QLabel;
    QFont scoreFont;
    scoreFont.setPointSize(18);
    scoreFont.setBold(true);
    m_scoreLabel->setFont(scoreFont);
    m_scoreLabel->setStyleSheet("color: #FFD700;");
    m_scoreLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_scoreLabel);

    m_statsLabel = new QLabel;
    m_statsLabel->setStyleSheet("color: #AAAACC; font-size: 12px;");
    m_statsLabel->setAlignment(Qt::AlignCenter);
    m_statsLabel->setWordWrap(true);
    layout->addWidget(m_statsLabel);

    auto* restartBtn = new QPushButton(QStringLiteral("再来一局 (Restart)"));
    restartBtn->setStyleSheet(
        "QPushButton { background: #2A5A2A; color: #AAFFAA; border: 2px solid #4A8A4A; "
        "border-radius: 6px; font-size: 13px; padding: 10px; }"
        "QPushButton:hover { background: #3A7A3A; }");
    layout->addWidget(restartBtn, 0, Qt::AlignCenter);

    auto* menuBtn = new QPushButton(QStringLiteral("返回主菜单 (Menu)"));
    menuBtn->setStyleSheet(
        "QPushButton { background: #3A3A5A; color: #CCCCFF; border: 2px solid #5A5A8A; "
        "border-radius: 6px; font-size: 12px; padding: 8px; }"
        "QPushButton:hover { background: #5A5A8A; }");
    layout->addWidget(menuBtn, 0, Qt::AlignCenter);

    connect(restartBtn, &QPushButton::clicked, this, [this]() {
        accept();
        emit restartRequested();
    });
    connect(menuBtn, &QPushButton::clicked, this, [this]() {
        accept();
        emit menuRequested();
    });

    if (m_engine) {
        m_scoreLabel->setText(QStringLiteral("Score: %1").arg(m_engine->score()));

        QStringList lines;
        lines << QStringLiteral("Level: %1  |  Lines: %2").arg(m_engine->level()).arg(m_engine->totalLines());
        lines << QStringLiteral("最高分：%1").arg(m_progressResult.snapshot.highScore);
        if (m_progressResult.isNewHighScore) {
            lines << QStringLiteral("新纪录已刷新。");
        }
        if (m_progressResult.enteredLeaderboard) {
            lines << QStringLiteral("已进入本地排行榜 Top %1。").arg(GameConfig::kLeaderboardSize);
        }
        if (!m_progressResult.newlyUnlockedSkillIds.isEmpty()) {
            QStringList unlockedNames;
            for (const QString& skillId : m_progressResult.newlyUnlockedSkillIds) {
                unlockedNames << m_engine->skillManager()->skillName(skillId);
            }
            lines << QStringLiteral("新解锁技能：%1").arg(unlockedNames.join(QStringLiteral("、")));
        }
        if (m_progressResult.newCarrySlots > m_progressResult.previousCarrySlots) {
            lines << QStringLiteral("挑战模式槽位提升到 %1。").arg(m_progressResult.newCarrySlots);
        }
        lines << QStringLiteral("技能使用次数：%1").arg(m_engine->skillManager()->totalSkillUses());

        m_statsLabel->setText(lines.join('\n'));
    }
}
