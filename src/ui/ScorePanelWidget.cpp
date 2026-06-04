#include "ScorePanelWidget.h"
#include "../core/GameEngine.h"
#include <QFont>

ScorePanelWidget::ScorePanelWidget(GameEngine* engine, QWidget* parent)
    : QWidget(parent), m_engine(engine)
{
    auto* layout = new QVBoxLayout(this);
    layout->setSpacing(8);
    layout->setContentsMargins(10, 10, 10, 10);

    auto makeTitleLabel = [](const QString& text) -> QLabel* {
        auto* label = new QLabel(text);
        QFont f = label->font();
        f.setPointSize(9);
        f.setBold(true);
        label->setFont(f);
        label->setStyleSheet("color: #8888BB;");
        return label;
    };

    auto makeValueLabel = [](const QString& text) -> QLabel* {
        auto* label = new QLabel(text);
        QFont f = label->font();
        f.setPointSize(18);
        f.setBold(true);
        label->setFont(f);
        label->setStyleSheet("color: #FFFFFF;");
        return label;
    };

    layout->addWidget(makeTitleLabel("Score"));
    m_scoreLabel = makeValueLabel("0");
    layout->addWidget(m_scoreLabel);

    layout->addWidget(makeTitleLabel("Level"));
    m_levelLabel = makeValueLabel("1");
    layout->addWidget(m_levelLabel);

    layout->addWidget(makeTitleLabel("Lines"));
    m_linesLabel = makeValueLabel("0");
    layout->addWidget(m_linesLabel);

    setMinimumWidth(130);
    setStyleSheet("background-color: #1E1E32; border-radius: 5px;");

    if (m_engine) {
        connect(m_engine, &GameEngine::scoreChanged,
                this, &ScorePanelWidget::updateLabels);
    }
}

void ScorePanelWidget::updateLabels(int score, int level, int lines)
{
    m_scoreLabel->setText(QString::number(score));
    m_levelLabel->setText(QString::number(level));
    m_linesLabel->setText(QString::number(lines));
}
