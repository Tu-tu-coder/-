#include "ProgressManager.h"

#include "GameConfig.h"

#include <algorithm>
#include <QDateTime>
#include <QSettings>

namespace {
const char* kOrgName = "CodexLocal";
const char* kAppName = "TetrisSkillBattle";
}

ProgressManager::ProgressManager()
{
    load();
}

ProgressUpdateResult ProgressManager::recordGameResult(int score, int level, int lines, const QString& mode)
{
    ProgressUpdateResult result;
    result.previousCarrySlots = m_snapshot.maxCarrySlots;
    const QStringList previousUnlocked = m_snapshot.unlockedSkillIds;

    if (score > m_snapshot.highScore) {
        m_snapshot.highScore = score;
        result.isNewHighScore = true;
    }

    ScoreEntry newEntry;
    newEntry.score = score;
    newEntry.level = level;
    newEntry.lines = lines;
    newEntry.mode = mode;
    newEntry.timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);

    QVector<ScoreEntry> updated = m_snapshot.topScores;
    updated.push_back(newEntry);
    std::sort(updated.begin(), updated.end(), [](const ScoreEntry& lhs, const ScoreEntry& rhs) {
        if (lhs.score != rhs.score) return lhs.score > rhs.score;
        return lhs.timestamp > rhs.timestamp;
    });
    if (updated.size() > GameConfig::kLeaderboardSize) {
        updated.resize(GameConfig::kLeaderboardSize);
    }
    result.enteredLeaderboard = std::any_of(updated.begin(), updated.end(), [&](const ScoreEntry& entry) {
        return entry.score == newEntry.score
            && entry.level == newEntry.level
            && entry.lines == newEntry.lines
            && entry.timestamp == newEntry.timestamp
            && entry.mode == newEntry.mode;
    });
    m_snapshot.topScores = updated;

    refreshUnlocks();
    result.newCarrySlots = m_snapshot.maxCarrySlots;
    for (const QString& skillId : m_snapshot.unlockedSkillIds) {
        if (!previousUnlocked.contains(skillId)) {
            result.newlyUnlockedSkillIds.push_back(skillId);
        }
    }
    result.snapshot = m_snapshot;
    save();
    return result;
}

void ProgressManager::saveSelectedLoadout(const QStringList& loadout)
{
    m_snapshot.selectedLoadoutSkillIds = sanitizeLoadout(loadout);
    save();
}

QString ProgressManager::skillUnlockSummary(const QString& skillId) const
{
    return QStringLiteral("%1 分解锁").arg(GameConfig::skillUnlockThreshold(skillId));
}

QString ProgressManager::highScoreSummary() const
{
    return QStringLiteral("最高分：%1").arg(m_snapshot.highScore);
}

void ProgressManager::load()
{
    QSettings settings(QString::fromUtf8(kOrgName), QString::fromUtf8(kAppName));
    m_snapshot.highScore = settings.value(QStringLiteral("progress/highScore"), 0).toInt();
    m_snapshot.topScores.clear();

    const int size = settings.beginReadArray(QStringLiteral("leaderboard"));
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        ScoreEntry entry;
        entry.score = settings.value(QStringLiteral("score"), 0).toInt();
        entry.level = settings.value(QStringLiteral("level"), 1).toInt();
        entry.lines = settings.value(QStringLiteral("lines"), 0).toInt();
        entry.timestamp = settings.value(QStringLiteral("timestamp")).toString();
        entry.mode = settings.value(QStringLiteral("mode")).toString();
        if (entry.score > 0) {
            m_snapshot.topScores.push_back(entry);
        }
    }
    settings.endArray();

    m_snapshot.selectedLoadoutSkillIds = settings.value(QStringLiteral("progress/selectedLoadout")).toStringList();
    refreshUnlocks();
}

void ProgressManager::save() const
{
    QSettings settings(QString::fromUtf8(kOrgName), QString::fromUtf8(kAppName));
    settings.setValue(QStringLiteral("progress/highScore"), m_snapshot.highScore);
    settings.setValue(QStringLiteral("progress/selectedLoadout"), m_snapshot.selectedLoadoutSkillIds);
    settings.beginWriteArray(QStringLiteral("leaderboard"));
    for (int i = 0; i < m_snapshot.topScores.size(); ++i) {
        settings.setArrayIndex(i);
        const ScoreEntry& entry = m_snapshot.topScores[i];
        settings.setValue(QStringLiteral("score"), entry.score);
        settings.setValue(QStringLiteral("level"), entry.level);
        settings.setValue(QStringLiteral("lines"), entry.lines);
        settings.setValue(QStringLiteral("timestamp"), entry.timestamp);
        settings.setValue(QStringLiteral("mode"), entry.mode);
    }
    settings.endArray();
}

void ProgressManager::refreshUnlocks()
{
    m_snapshot.unlockedSkillIds = GameConfig::unlockedSkillsForHighScore(m_snapshot.highScore);
    m_snapshot.maxCarrySlots = GameConfig::carrySlotsForHighScore(m_snapshot.highScore);
    m_snapshot.selectedLoadoutSkillIds = sanitizeLoadout(m_snapshot.selectedLoadoutSkillIds);
}

QStringList ProgressManager::sanitizeLoadout(const QStringList& loadout) const
{
    QStringList result;
    for (const QString& skillId : loadout) {
        if (!m_snapshot.unlockedSkillIds.contains(skillId)) continue;
        if (result.contains(skillId)) continue;
        result.push_back(skillId);
        if (result.size() >= m_snapshot.maxCarrySlots) {
            break;
        }
    }

    for (const QString& skillId : m_snapshot.unlockedSkillIds) {
        if (result.size() >= m_snapshot.maxCarrySlots) {
            break;
        }
        if (!result.contains(skillId)) {
            result.push_back(skillId);
        }
    }

    return result;
}

QStringList ProgressManager::defaultLoadout() const
{
    return sanitizeLoadout({});
}
