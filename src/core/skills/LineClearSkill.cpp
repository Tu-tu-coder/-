#include "LineClearSkill.h"
#include "GameConfig.h"
#include "GameEngine.h"
#include "SkillFactory.h"
#include "TetrisBoard.h"

void LineClearSkill::onActivate(TetrisBoard& board, GameEngine& engine)
{
    Q_UNUSED(engine);
    int targetRow = -1;
    int bestVisibleRow = GameConfig::kBoardHeight;

    for (int visibleRow = 0; visibleRow < GameConfig::kBoardHeight; ++visibleRow) {
        bool rowOccupied = false;
        for (int col = 0; col < board.width(); ++col) {
            if (board.cellAtVisible(visibleRow, col) != GameConfig::kEmptyCell) {
                rowOccupied = true;
                break;
            }
        }
        if (rowOccupied && visibleRow < bestVisibleRow) {
            bestVisibleRow = visibleRow;
            targetRow = visibleRow + GameConfig::kBufferRows;
        }
    }

    if (targetRow >= 0) {
        board.clearRow(targetRow);
        board.applyGravity();
    }
}

REGISTER_SKILL(LineClearSkill, "line_clear")
