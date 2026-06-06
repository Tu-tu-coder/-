#ifndef MENUWIDGET_H
#define MENUWIDGET_H

#include <QHash>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "../core/GameEngine.h"
#include "../core/ProgressManager.h"

class SkillManager;

class MenuWidget : public QWidget {
    Q_OBJECT
public:
    explicit MenuWidget(SkillManager* skillMgr, QWidget* parent = nullptr);

    void setProgressSnapshot(const ProgressSnapshot& snapshot);

signals:
    void startGame(GameMode mode, const QStringList& selectedLoadoutSkillIds);

private:
    void rebuildSkillButtons();
    void updateLoadoutUI();
    void onSkillButtonClicked(const QString& skillId);
    QString skillButtonText(const QString& skillId) const;

    SkillManager* m_skillMgr = nullptr;
    ProgressSnapshot m_snapshot;
    QStringList m_selectedLoadoutSkillIds;
    QLabel* m_progressLabel = nullptr;
    QLabel* m_leaderboardLabel = nullptr;
    QLabel* m_loadoutTitleLabel = nullptr;
    QLabel* m_loadoutHintLabel = nullptr;
    QWidget* m_loadoutContainer = nullptr;
    QVBoxLayout* m_loadoutLayout = nullptr;
    QHash<QString, QPushButton*> m_skillButtons;
    QPushButton* m_challengeBtn = nullptr;
    QPushButton* m_classicBtn = nullptr;
};

#endif
