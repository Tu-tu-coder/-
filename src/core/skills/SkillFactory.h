#ifndef SKILLFACTORY_H
#define SKILLFACTORY_H

#include <QString>
#include <QHash>
#include <functional>
#include <memory>

class SkillBase;

// Singleton registry for skill types.
// Skills register themselves at static init time via REGISTER_SKILL macro.
// To list available skills (e.g. for UI), use registeredSkills().
class SkillFactory {
public:
    using Creator = std::function<std::unique_ptr<SkillBase>()>;

    static SkillFactory& instance();

    // Register a skill type (called by REGISTER_SKILL macro)
    void registerSkill(const QString& typeId, Creator creator);

    // Create a skill instance by type ID
    std::unique_ptr<SkillBase> create(const QString& typeId) const;

    // Get list of all registered skill type IDs
    QStringList registeredSkills() const;

    // Check if a skill ID is registered
    bool isRegistered(const QString& typeId) const;

private:
    SkillFactory() = default;
    QHash<QString, Creator> m_registry;
};

// Macro to auto-register a skill at static init time
// Usage: put at the bottom of your skill's .cpp file
//         REGISTER_SKILL(TimeFreezeSkill, "time_freeze")
#define REGISTER_SKILL(ClassName, TypeId) \
    namespace { \
        static bool _reg_##ClassName = []() -> bool { \
            SkillFactory::instance().registerSkill( \
                TypeId, \
                []() -> std::unique_ptr<SkillBase> { \
                    return std::make_unique<ClassName>(); \
                }); \
            return true; \
        }(); \
    }

#endif // SKILLFACTORY_H
