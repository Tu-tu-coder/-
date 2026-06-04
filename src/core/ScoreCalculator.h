#ifndef SCORECALCULATOR_H
#define SCORECALCULATOR_H

#include "GameConfig.h"
#include <QObject>

class ScoreCalculator : public QObject {
    Q_OBJECT
public:
    explicit ScoreCalculator(QObject* parent = nullptr);

    void reset();

    // Add line clear points. nLines = number of rows cleared simultaneously.
    // Returns the points earned for this clear.
    int addLineClear(int nLines);

    // Add soft/hard drop bonus
    void addSoftDrop(int cells);
    void addHardDrop(int cells);

    // Reset combo counter (call when piece locks without clearing lines)
    void resetCombo();

    // Accessors
    int score() const { return m_score; }
    int level() const { return m_level; }
    int totalLines() const { return m_totalLines; }
    int combo() const { return m_combo; }

signals:
    void scoreChanged(int score, int level, int lines);
    void levelUp(int newLevel);

private:
    void addScore(int points);
    void checkLevelUp();

    int m_score = 0;
    int m_level = 1;
    int m_totalLines = 0;
    int m_combo = 0; // consecutive clears with no reset
};

#endif // SCORECALCULATOR_H
