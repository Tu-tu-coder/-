#include "BottomBlastSkill.h"
#include "GameEngine.h"
#include "SkillFactory.h"
#include "TetrisBoard.h"

void BottomBlastSkill::onActivate(TetrisBoard& board, GameEngine& engine)
{
    Q_UNUSED(engine);
    board.clearRow(board.totalRows() - 1);
    board.applyGravity();
}

REGISTER_SKILL(BottomBlastSkill, "bottom_blast")
