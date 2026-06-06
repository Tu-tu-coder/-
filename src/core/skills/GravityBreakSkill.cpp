#include "GravityBreakSkill.h"
#include "GameEngine.h"
#include "SkillFactory.h"
#include "TetrisBoard.h"

void GravityBreakSkill::onActivate(TetrisBoard& board, GameEngine& engine)
{
    Q_UNUSED(board);
    Q_UNUSED(engine);
    m_remainingMs = duration();
}

void GravityBreakSkill::onTick(TetrisBoard& board, GameEngine& engine, qint64 deltaMs)
{
    Q_UNUSED(board);
    Q_UNUSED(engine);
    m_remainingMs -= deltaMs;
}

void GravityBreakSkill::onDeactivate(TetrisBoard& board, GameEngine& engine)
{
    Q_UNUSED(board);
    Q_UNUSED(engine);
    m_remainingMs = 0;
}

REGISTER_SKILL(GravityBreakSkill, "gravity_break")
