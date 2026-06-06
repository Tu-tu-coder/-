#include "SkillManager.h"

#include "GameEngine.h"
#include "TetrisBoard.h"
#include "skills/SkillFactory.h"

SkillManager::SkillManager(QObject* parent)
    : QObject(parent)
{
}

void SkillManager::reset()
{
    m_energy = 0;
    m_activeSkill.reset();
    m_activeSkillId.clear();
    emit energyChanged(m_energy, maxEnergy());
    emit energyFull(false);
}

void SkillManager::addEnergy(int amount)
{
    if (!m_engine || !m_engine->isSkillsEnabled()) return;

    const int oldEnergy = m_energy;
    m_energy = qMin(m_energy + amount, maxEnergy());

    if (m_energy != oldEnergy) {
        emit energyChanged(m_energy, maxEnergy());
        emit energyFull(m_energy >= maxEnergy());
    }
}

void SkillManager::setUnlockedSkills(const QStringList& skillIds)
{
    m_unlockedSkillIds = skillIds;
    emit loadoutChanged();
}

void SkillManager::setLoadoutSkills(const QStringList& skillIds)
{
    m_loadoutSkillIds = skillIds;
    if (m_defaultSlot >= m_loadoutSkillIds.size()) {
        m_defaultSlot = 0;
    }
    emit loadoutChanged();
}

void SkillManager::setDefaultSlot(int slot)
{
    if (slot < 0 || slot >= m_loadoutSkillIds.size() || slot == m_defaultSlot) {
        return;
    }
    m_defaultSlot = slot;
    emit defaultSlotChanged(slot);
}

bool SkillManager::tryActivateSlot(int slot, TetrisBoard& board)
{
    if (!isSlotUnlocked(slot)) return false;
    setDefaultSlot(slot);
    return tryActivateSkillId(m_loadoutSkillIds[slot], board);
}

bool SkillManager::canActivateSlot(int slot) const
{
    if (!isSlotUnlocked(slot) || m_activeSkill) return false;
    return m_energy >= skillEnergyCost(m_loadoutSkillIds[slot]);
}

bool SkillManager::isSlotUnlocked(int slot) const
{
    return slot >= 0 && slot < m_loadoutSkillIds.size();
}

bool SkillManager::tryActivateSkillId(const QString& skillId, TetrisBoard& board)
{
    if (m_activeSkill) return false;
    if (skillId.isEmpty()) return false;

    const int cost = skillEnergyCost(skillId);
    if (m_energy < cost) return false;

    m_energy -= cost;
    emit energyChanged(m_energy, maxEnergy());
    emit energyFull(false);

    m_activeSkill = SkillFactory::instance().create(skillId);
    if (!m_activeSkill) return false;

    m_activeSkillId = skillId;
    m_skillUseCounts[skillId]++;
    m_totalSkillUses++;

    emit skillActivated(skillId);
    m_activeSkill->onActivate(board, *m_engine);

    if (m_activeSkill->isInstant()) {
        emit skillDeactivated(skillId);
        m_activeSkill.reset();
        m_activeSkillId.clear();
    }

    return true;
}

void SkillManager::update(qint64 deltaMs)
{
    if (!m_activeSkill || !m_engine) return;

    m_activeSkill->onTick(m_engine->board(), *m_engine, deltaMs);

    if (m_activeSkill->isExpired()) {
        m_activeSkill->onDeactivate(m_engine->board(), *m_engine);
        emit skillDeactivated(m_activeSkillId);
        m_activeSkill.reset();
        m_activeSkillId.clear();
    }
}

float SkillManager::gravityMultiplier() const
{
    return m_activeSkill ? m_activeSkill->gravityMultiplier() : 1.0f;
}

QStringList SkillManager::availableSkills() const
{
    return GameConfig::skillUnlockOrder();
}

QString SkillManager::skillName(const QString& skillId) const
{
    auto skill = SkillFactory::instance().create(skillId);
    return skill ? skill->name() : skillId;
}

QString SkillManager::skillDescription(const QString& skillId) const
{
    auto skill = SkillFactory::instance().create(skillId);
    return skill ? skill->description() : QString();
}

QColor SkillManager::skillColor(const QString& skillId) const
{
    auto skill = SkillFactory::instance().create(skillId);
    return skill ? skill->themeColor() : QColor(128, 128, 128);
}

int SkillManager::skillEnergyCost(const QString& skillId) const
{
    auto skill = SkillFactory::instance().create(skillId);
    return skill ? skill->energyCost() : maxEnergy();
}

int SkillManager::skillUseCount(const QString& skillId) const
{
    return m_skillUseCounts.value(skillId, 0);
}
