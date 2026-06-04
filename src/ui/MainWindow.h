#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QStackedWidget>
#include <QElapsedTimer>
#include "../core/GameEngine.h"
#include "../core/SkillManager.h"

class GameBoardWidget;
class NextPieceWidget;
class ScorePanelWidget;
class EnergyBarWidget;
class SkillPanelWidget;
class MenuWidget;
class GameOverDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onGameTick();
    void onStartGame(GameMode mode, const QString& skillId);
    void onGameOver();
    void showMenu();

private:
    void setupUI();
    void setupConnections();
    void showGameUI();
    void showMenuUI();
    void startGameTimer();
    void stopGameTimer();

    // Core
    GameEngine* m_engine;
    SkillManager* m_skillMgr;

    // UI pages
    QStackedWidget* m_stack;
    QWidget* m_gamePage;
    MenuWidget* m_menuPage;

    // Game UI components
    GameBoardWidget* m_boardWidget;
    NextPieceWidget* m_nextPiece;
    ScorePanelWidget* m_scorePanel;
    EnergyBarWidget* m_energyBar;
    SkillPanelWidget* m_skillPanel;

    // Timer
    QTimer* m_gameTimer;
    QElapsedTimer m_frameTimer;
};

#endif // MAINWINDOW_H
