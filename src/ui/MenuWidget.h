#ifndef MENUWIDGET_H
#define MENUWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QLabel>
#include "../core/GameEngine.h"

class SkillManager;

class MenuWidget : public QWidget {
    Q_OBJECT
public:
    explicit MenuWidget(SkillManager* skillMgr, QWidget* parent = nullptr);

signals:
    void startGame(GameMode mode, const QString& skillId);
    void quitRequested();

private:
    void onStartClassic();
    void onStartChallenge();

    SkillManager* m_skillMgr = nullptr;
    QComboBox* m_skillCombo;
    QPushButton* m_challengeBtn;
    QPushButton* m_classicBtn;
};

#endif
