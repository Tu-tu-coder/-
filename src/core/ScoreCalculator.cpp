#include "ScoreCalculator.h"

ScoreCalculator::ScoreCalculator(QObject* parent)
    : QObject(parent)
{
}

void ScoreCalculator::reset()
{
    m_score = 0;
    m_level = 1;
    m_totalLines = 0;
    m_combo = 0;
    emit scoreChanged(m_score, m_level, m_totalLines);
}

int ScoreCalculator::addLineClear(int nLines)
{
    int basePoints = 0;
    switch (nLines) {
        case 1: basePoints = GameConfig::kScoreSingle;   break;
        case 2: basePoints = GameConfig::kScoreDouble;   break;
        case 3: basePoints = GameConfig::kScoreTriple;   break;
        case 4: basePoints = GameConfig::kScoreTetris;   break;
        default: basePoints = 0; break;
    }

    int points = basePoints * m_level;

    // Combo bonus: consecutive clears get extra points
    if (m_combo > 0) {
        points += GameConfig::kScoreCombo * m_combo * m_level;
    }
    ++m_combo;

    m_totalLines += nLines;
    addScore(points);
    checkLevelUp();

    return points;
}

void ScoreCalculator::addSoftDrop(int cells)
{
    addScore(cells * GameConfig::kScoreSoftDrop);
}

void ScoreCalculator::addHardDrop(int cells)
{
    addScore(cells * GameConfig::kScoreHardDrop);
}

void ScoreCalculator::resetCombo()
{
    m_combo = 0;
}

void ScoreCalculator::addScore(int points)
{
    m_score += points;
    emit scoreChanged(m_score, m_level, m_totalLines);
}

void ScoreCalculator::checkLevelUp()
{
    int newLevel = (m_totalLines / GameConfig::kLinesPerLevel) + 1;
    if (newLevel > m_level && newLevel <= GameConfig::kMaxLevel) {
        m_level = newLevel;
        emit levelUp(m_level);
        emit scoreChanged(m_score, m_level, m_totalLines);
    }
}
