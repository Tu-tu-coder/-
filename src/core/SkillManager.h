#ifndef SKILLMANAGER_H
#define SKILLMANAGER_H

#include <QObject>
#include <memory>
#include "skills/SkillBase.h"

class TetrisBoard;
class GameEngine;

class SkillManager : public QObject {
    Q_OBJECT
public:
    explicit SkillManager(QObject* parent = nullptr);

    // Energy
    int energy() const { return m_energy; }
    int maxEnergy() const { return 100; }
    bool isEnergyFull() const { return m_energy >= maxEnergy(); }
    void addEnergy(int amount);
    void reset();

    // Skill
    QString equippedSkillId() const { return m_equippedSkillId; }
    void equipSkill(const QString& skillId);

    bool isSkillActive() const { return m_activeSkill != nullptr; }
    float gravityMultiplier() const;

    // Try to activate equipped skill. Returns true if activated.
    bool tryActivate(TetrisBoard& board);

    // Called each frame by GameEngine
    void update(qint64 deltaMs);

    // Get list of all available skills (for UI)
    QStringList availableSkills() const;

    // Get info for a registered skill
    QString skillName(const QString& skillId) const;
    QString skillDescription(const QString& skillId) const;
    QColor skillColor(const QString& skillId) const;
    int skillEnergyCost(const QString& skillId) const;

    // Skill usage stats
    int skillUseCount(const QString& skillId) const;
    int totalSkillUses() const { return m_totalSkillUses; }

signals:
    void energyChanged(int current, int max);
    void energyFull(bool full);
    void skillActivated(const QString& skillName);
    void skillDeactivated(const QString& skillName);
    void skillEquipped(const QString& skillId);

private:
    void activateSkill(TetrisBoard& board);
    void deactivateSkill(TetrisBoard& board);

    int m_energy = 0;
    QString m_equippedSkillId;
    std::unique_ptr<SkillBase> m_activeSkill;
    GameEngine* m_engine = nullptr; // set by GameEngine after construction

    // Stats
    QHash<QString, int> m_skillUseCounts;
    int m_totalSkillUses = 0;

    friend class GameEngine;
};

#endif // SKILLMANAGER_H
