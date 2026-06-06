#include "PurgeWaveSkill.h"
#include "GameEngine.h"
#include "SkillFactory.h"
#include "TetrisBoard.h"

void PurgeWaveSkill::onActivate(TetrisBoard& board, GameEngine& engine)
{
    Q_UNUSED(engine);
    for (int i = 0; i < 4; ++i) {
        board.clearRow(board.totalRows() - 1);
    }
    board.applyGravity();
}

REGISTER_SKILL(PurgeWaveSkill, "purge_wave")
