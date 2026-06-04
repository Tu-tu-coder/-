#include "GameOverDialog.h"
#include "../core/GameEngine.h"
#include "../core/SkillManager.h"
#include <QVBoxLayout>

GameOverDialog::GameOverDialog(GameEngine* engine, QWidget* parent)
    : QDialog(parent), m_engine(engine)
{
    setWindowTitle("Game Over");
    setFixedSize(350, 350);
    setStyleSheet("background-color: #1A1A35; color: white;");

    auto* layout = new QVBoxLayout(this);
    layout->setSpacing(12);
    layout->setAlignment(Qt::AlignCenter);

    // Title
    auto* title = new QLabel("游戏结束");
    QFont tf;
    tf.setPointSize(22);
    tf.setBold(true);
    title->setFont(tf);
    title->setStyleSheet("color: #FF6B6B;");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    layout->addSpacing(10);

    // Score
    m_scoreLabel = new QLabel;
    QFont sf;
    sf.setPointSize(16);
    sf.setBold(true);
    m_scoreLabel->setFont(sf);
    m_scoreLabel->setStyleSheet("color: #FFD700;");
    m_scoreLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_scoreLabel);

    // Stats
    m_statsLabel = new QLabel;
    m_statsLabel->setStyleSheet("color: #AAAACC; font-size: 12px;");
    m_statsLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_statsLabel);

    layout->addSpacing(20);

    // Restart button
    auto* restartBtn = new QPushButton("再来一局 (Restart)");
    restartBtn->setStyleSheet(
        "QPushButton { background: #2A5A2A; color: #AAFFAA; border: 2px solid #4A8A4A; "
        "border-radius: 6px; font-size: 13px; padding: 10px; }"
        "QPushButton:hover { background: #3A7A3A; }");
    layout->addWidget(restartBtn, 0, Qt::AlignCenter);

    // Menu button
    auto* menuBtn = new QPushButton("返回主菜单 (Menu)");
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

    // Fill data
    if (m_engine) {
        m_scoreLabel->setText(QString("Score: %1").arg(m_engine->score()));

        QString stats = QString("Level: %1  |  Lines: %2")
            .arg(m_engine->level())
            .arg(m_engine->totalLines());

        if (m_engine->skillManager()) {
            int uses = m_engine->skillManager()->totalSkillUses();
            stats += QString("\nSkills Used: %1 times").arg(uses);

            if (!m_engine->skillManager()->equippedSkillId().isEmpty()) {
                QString id = m_engine->skillManager()->equippedSkillId();
                stats += QString("\nSkill: %1 (%2 uses)")
                    .arg(m_engine->skillManager()->skillName(id))
                    .arg(m_engine->skillManager()->skillUseCount(id));
            }
        }

        m_statsLabel->setText(stats);
    }
}
