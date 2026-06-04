#include "SkillFactory.h"
#include "SkillBase.h"

SkillFactory& SkillFactory::instance()
{
    static SkillFactory factory;
    return factory;
}

void SkillFactory::registerSkill(const QString& typeId, Creator creator)
{
    m_registry[typeId] = std::move(creator);
}

std::unique_ptr<SkillBase> SkillFactory::create(const QString& typeId) const
{
    auto it = m_registry.find(typeId);
    if (it != m_registry.end()) {
        return it.value()();
    }
    return nullptr;
}

QStringList SkillFactory::registeredSkills() const
{
    return m_registry.keys();
}

bool SkillFactory::isRegistered(const QString& typeId) const
{
    return m_registry.contains(typeId);
}
