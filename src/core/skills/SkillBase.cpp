#include "SkillBase.h"

QJsonObject SkillBase::toJson() const
{
    QJsonObject obj;
    obj["typeId"] = typeId();
    obj["name"] = name();
    obj["description"] = description();
    obj["themeColor"] = themeColor().name();
    obj["isInstant"] = isInstant();
    obj["duration"] = static_cast<double>(duration());
    obj["energyCost"] = energyCost();
    return obj;
}

void SkillBase::fromJson(const QJsonObject& json)
{
    Q_UNUSED(json);
    // Base implementation reads nothing — subclasses override if needed
}
