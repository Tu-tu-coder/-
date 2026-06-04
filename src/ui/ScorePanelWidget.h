#ifndef SCOREPANELWIDGET_H
#define SCOREPANELWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class GameEngine;

class ScorePanelWidget : public QWidget {
    Q_OBJECT
public:
    explicit ScorePanelWidget(GameEngine* engine, QWidget* parent = nullptr);

private:
    void updateLabels(int score, int level, int lines);

    QLabel* m_scoreLabel;
    QLabel* m_levelLabel;
    QLabel* m_linesLabel;
    GameEngine* m_engine = nullptr;
};

#endif
