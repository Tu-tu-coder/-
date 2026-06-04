#ifndef GAMEOVERDIALOG_H
#define GAMEOVERDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>

class GameEngine;

class GameOverDialog : public QDialog {
    Q_OBJECT
public:
    explicit GameOverDialog(GameEngine* engine, QWidget* parent = nullptr);

signals:
    void restartRequested();
    void menuRequested();

private:
    GameEngine* m_engine = nullptr;
    QLabel* m_scoreLabel;
    QLabel* m_statsLabel;
};

#endif
