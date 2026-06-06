#ifndef SKILLMANAGER_H
#define SKILLMANAGER_H

#include <QObject>
#include <memory>

#include "GameConfig.h"
#include "skills/SkillBase.h"

class TetrisBoard;
class GameEngine;

class SkillManager : public QObject {
    Q_OBJECT
public:
    explicit SkillManager(QObject* parent = nullptr);

    int energy() const { return m_energy; }
    int maxEnergy() const { return GameConfig::kEnergyMax; }
    bool isEnergyFull() const { return m_energy >= maxEnergy(); }
    void addEnergy(int amount);
    void reset();

    void setUnlockedSkills(const QStringList& skillIds);
    void setLoadoutSkills(const QStringList& skillIds);
    QStringList unlockedSkills() const { return m_unlockedSkillIds; }
    QStringList loadoutSkills() const { return m_loadoutSkillIds; }
    QString activeSkillId() const { return m_activeSkillId; }
    int defaultSlot() const { return m_defaultSlot; }
    void setDefaultSlot(int slot);

    bool isSkillActive() const { return m_activeSkill != nullptr; }
    float gravityMultiplier() const;

    bool tryActivateSlot(int slot, TetrisBoard& board);
    bool canActivateSlot(int slot) const;
    bool isSlotUnlocked(int slot) const;

    void update(qint64 deltaMs);

    QStringList availableSkills() const;
    QString skillName(const QString& skillId) const;
    QString skillDescription(const QString& skillId) const;
    QColor skillColor(const QString& skillId) const;
    int skillEnergyCost(const QString& skillId) const;

    int skillUseCount(const QString& skillId) const;
    int totalSkillUses() const { return m_totalSkillUses; }

signals:
    void energyChanged(int current, int max);
    void energyFull(bool full);
    void skillActivated(const QString& skillId);
    void skillDeactivated(const QString& skillId);
    void loadoutChanged();
    void defaultSlotChanged(int slot);

private:
    bool tryActivateSkillId(const QString& skillId, TetrisBoard& board);

    int m_energy = 0;
    QStringList m_unlockedSkillIds;
    QStringList m_loadoutSkillIds;
    std::unique_ptr<SkillBase> m_activeSkill;
    QString m_activeSkillId;
    int m_defaultSlot = 0;
    GameEngine* m_engine = nullptr;

    QHash<QString, int> m_skillUseCounts;
    int m_totalSkillUses = 0;

    friend class GameEngine;
};

#endif // SKILLMANAGER_H
