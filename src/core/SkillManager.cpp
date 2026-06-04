#include "SkillManager.h"
#include "skills/SkillFactory.h"
#include "TetrisBoard.h"
#include "GameEngine.h"
#include "GameConfig.h"

SkillManager::SkillManager(QObject* parent)
    : QObject(parent)
{
}

void SkillManager::reset()
{
    m_energy = 0;
    m_activeSkill.reset();
    emit energyChanged(m_energy, maxEnergy());
    emit energyFull(false);
}

void SkillManager::addEnergy(int amount)
{
    if (!m_engine || !m_engine->isSkillsEnabled()) return;

    int oldEnergy = m_energy;
    m_energy = qMin(m_energy + amount, maxEnergy());

    if (m_energy != oldEnergy) {
        emit energyChanged(m_energy, maxEnergy());
        if (m_energy >= maxEnergy() && oldEnergy < maxEnergy()) {
            emit energyFull(true);
        }
    }
}

void SkillManager::equipSkill(const QString& skillId)
{
    m_equippedSkillId = skillId;
    emit skillEquipped(skillId);
}

bool SkillManager::tryActivate(TetrisBoard& board)
{
    if (m_activeSkill) return false; // skill already active
    if (m_equippedSkillId.isEmpty()) return false;

    // Check if we have enough energy
    int cost = 100; // default
    auto testSkill = SkillFactory::instance().create(m_equippedSkillId);
    if (testSkill) {
        cost = testSkill->energyCost();
    }

    if (m_energy < cost) return false;

    // Consume energy
    m_energy -= cost;
    emit energyChanged(m_energy, maxEnergy());
    emit energyFull(false);

    // Create and activate
    m_activeSkill = SkillFactory::instance().create(m_equippedSkillId);
    if (!m_activeSkill) return false;

    // Record stats
    m_skillUseCounts[m_equippedSkillId]++;
    m_totalSkillUses++;

    emit skillActivated(m_activeSkill->name());

    // Call activate (for instant skills, deactivate immediately after)
    m_activeSkill->onActivate(board, *m_engine);

    if (m_activeSkill->isInstant()) {
        emit skillDeactivated(m_activeSkill->name());
        m_activeSkill.reset();
    }

    return true;
}

void SkillManager::update(qint64 deltaMs)
{
    if (!m_activeSkill || !m_engine) return;

    m_activeSkill->onTick(m_engine->board(), *m_engine, deltaMs);

    // Check expiry
    if (m_activeSkill->isExpired()) {
        m_activeSkill->onDeactivate(m_engine->board(), *m_engine);
        emit skillDeactivated(m_activeSkill->name());
        m_activeSkill.reset();
    }
}

void SkillManager::activateSkill(TetrisBoard& board)
{
    if (!m_activeSkill) return;
    m_activeSkill->onActivate(board, *m_engine);
}

void SkillManager::deactivateSkill(TetrisBoard& board)
{
    if (!m_activeSkill) return;
    m_activeSkill->onDeactivate(board, *m_engine);
    emit skillDeactivated(m_activeSkill->name());
    m_activeSkill.reset();
}

float SkillManager::gravityMultiplier() const
{
    if (m_activeSkill) {
        return m_activeSkill->gravityMultiplier();
    }
    return 1.0f;
}

QStringList SkillManager::availableSkills() const
{
    return SkillFactory::instance().registeredSkills();
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
    return skill ? skill->energyCost() : 100;
}

int SkillManager::skillUseCount(const QString& skillId) const
{
    return m_skillUseCounts.value(skillId, 0);
}
