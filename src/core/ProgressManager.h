#ifndef PROGRESSMANAGER_H
#define PROGRESSMANAGER_H

#include <QString>
#include <QStringList>
#include <QVector>

struct ScoreEntry {
    int score = 0;
    int level = 1;
    int lines = 0;
    QString timestamp;
    QString mode;
};

struct ProgressSnapshot {
    int highScore = 0;
    QVector<ScoreEntry> topScores;
    QStringList unlockedSkillIds;
    int maxCarrySlots = 2;
    QStringList selectedLoadoutSkillIds;
};

struct ProgressUpdateResult {
    bool isNewHighScore = false;
    bool enteredLeaderboard = false;
    QStringList newlyUnlockedSkillIds;
    int previousCarrySlots = 2;
    int newCarrySlots = 2;
    ProgressSnapshot snapshot;
};

class ProgressManager {
public:
    ProgressManager();

    const ProgressSnapshot& snapshot() const { return m_snapshot; }
    ProgressUpdateResult recordGameResult(int score, int level, int lines, const QString& mode);
    void saveSelectedLoadout(const QStringList& loadout);
    QString skillUnlockSummary(const QString& skillId) const;
    QString highScoreSummary() const;

private:
    void load();
    void save() const;
    void refreshUnlocks();
    QStringList sanitizeLoadout(const QStringList& loadout) const;
    QStringList defaultLoadout() const;

    ProgressSnapshot m_snapshot;
};

#endif
