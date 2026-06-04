#include "TimeFreezeSkill.h"
#include "TetrisBoard.h"
#include "GameEngine.h"
#include "SkillFactory.h"

void TimeFreezeSkill::onActivate(TetrisBoard& board, GameEngine& engine)
{
    Q_UNUSED(board);
    Q_UNUSED(engine);
    m_remainingMs = duration();
}

void TimeFreezeSkill::onTick(TetrisBoard& board, GameEngine& engine, qint64 deltaMs)
{
    Q_UNUSED(board);
    Q_UNUSED(engine);
    m_remainingMs -= deltaMs;
}

void TimeFreezeSkill::onDeactivate(TetrisBoard& board, GameEngine& engine)
{
    Q_UNUSED(board);
    Q_UNUSED(engine);
    m_remainingMs = 0;
}

REGISTER_SKILL(TimeFreezeSkill, "time_freeze")
