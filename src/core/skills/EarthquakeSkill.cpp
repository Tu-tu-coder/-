#include "EarthquakeSkill.h"
#include "TetrisBoard.h"
#include "GameEngine.h"
#include "SkillFactory.h"
#include "GameConfig.h"

void EarthquakeSkill::onActivate(TetrisBoard& board, GameEngine& engine)
{
    Q_UNUSED(engine);

    int totalRows = board.totalRows();

    // Clear bottom 2 rows
    int bottomRow1 = totalRows - 1;
    int bottomRow2 = totalRows - 2;

    board.clearRow(bottomRow1);
    board.clearRow(bottomRow2);

    // Apply gravity to let floating blocks fall
    board.applyGravity();
}

REGISTER_SKILL(EarthquakeSkill, "earthquake")
