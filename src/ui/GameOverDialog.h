#ifndef GAMEOVERDIALOG_H
#define GAMEOVERDIALOG_H

#include <QDialog>
#include <QLabel>

#include "../core/ProgressManager.h"

class GameEngine;

class GameOverDialog : public QDialog {
    Q_OBJECT
public:
    explicit GameOverDialog(GameEngine* engine, const ProgressUpdateResult& progressResult, QWidget* parent = nullptr);

signals:
    void restartRequested();
    void menuRequested();

private:
    GameEngine* m_engine = nullptr;
    ProgressUpdateResult m_progressResult;
    QLabel* m_scoreLabel = nullptr;
    QLabel* m_statsLabel = nullptr;
};

#endif // GAMEOVERDIALOG_H
