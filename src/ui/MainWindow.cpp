#include "MainWindow.h"
#include "GameBoardWidget.h"
#include "NextPieceWidget.h"
#include "ScorePanelWidget.h"
#include "EnergyBarWidget.h"
#include "SkillPanelWidget.h"
#include "MenuWidget.h"
#include "GameOverDialog.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QAction>
#include <QApplication>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    m_engine = new GameEngine(this);
    m_skillMgr = new SkillManager(this);
    m_engine->setSkillManager(m_skillMgr);

    setupUI();
    setupConnections();
    showMenuUI();
    setStyleSheet("QMainWindow { background-color: #12122A; }");
}

MainWindow::~MainWindow()
{
    stopGameTimer();
}

void MainWindow::setupUI()
{
    // === Menu Bar ===
    auto* gameMenu = menuBar()->addMenu("Game");
    QAction* menuAction = gameMenu->addAction("Main Menu");
    connect(menuAction, &QAction::triggered, this, &MainWindow::showMenu);
    QAction* quitAction = gameMenu->addAction("Quit");
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

    // Apply menu bar styling
    menuBar()->setStyleSheet(
        "QMenuBar { background: #1A1A35; color: #CCCCEE; padding: 2px; }"
        "QMenuBar::item:selected { background: #3A3A6A; }"
        "QMenu { background: #1A1A35; color: #CCCCEE; border: 1px solid #444; }"
        "QMenu::item:selected { background: #3A3A6A; }"
    );

    // === Stacked widget (menu / game) ===
    m_stack = new QStackedWidget(this);
    setCentralWidget(m_stack);

    // --- Menu page ---
    m_menuPage = new MenuWidget(m_skillMgr);
    m_stack->addWidget(m_menuPage); // index 0

    // --- Game page ---
    m_gamePage = new QWidget;
    auto* gameLayout = new QHBoxLayout(m_gamePage);
    gameLayout->setContentsMargins(10, 10, 10, 10);
    gameLayout->setSpacing(10);

    // Left: game board
    m_boardWidget = new GameBoardWidget(m_engine);
    gameLayout->addWidget(m_boardWidget, 3); // stretch factor 3

    // Right: info panel
    auto* rightPanel = new QWidget;
    auto* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setSpacing(10);
    rightLayout->setContentsMargins(0, 0, 0, 0);

    m_nextPiece = new NextPieceWidget(m_engine);
    rightLayout->addWidget(m_nextPiece);

    m_scorePanel = new ScorePanelWidget(m_engine);
    rightLayout->addWidget(m_scorePanel);

    m_energyBar = new EnergyBarWidget(m_skillMgr);
    rightLayout->addWidget(m_energyBar);

    m_skillPanel = new SkillPanelWidget(m_skillMgr, m_engine);
    rightLayout->addWidget(m_skillPanel);

    rightLayout->addStretch();
    gameLayout->addWidget(rightPanel, 1); // stretch factor 1

    m_stack->addWidget(m_gamePage); // index 1

    // === Game Timer ===
    m_gameTimer = new QTimer(this);
    m_gameTimer->setInterval(GameConfig::kFrameInterval); // ~60 FPS
}

void MainWindow::setupConnections()
{
    connect(m_menuPage, &MenuWidget::startGame,
            this, &MainWindow::onStartGame);

    connect(m_engine, &GameEngine::gameStateChanged, this, [this](GameState state) {
        if (state == GameState::GameOver) {
            stopGameTimer();
            onGameOver();
        }
    });

    connect(m_gameTimer, &QTimer::timeout, this, &MainWindow::onGameTick);
}

void MainWindow::onStartGame(GameMode mode, const QStringList& selectedLoadoutSkillIds)
{
    if (mode == GameMode::InfiniteChallenge) {
        m_progress.saveSelectedLoadout(selectedLoadoutSkillIds);
    }
    configureSkillsForMode(mode, selectedLoadoutSkillIds);

    m_engine->startGame(mode);
    m_frameTimer.start();
    showGameUI();
    startGameTimer();
    m_boardWidget->setFocus();
}

void MainWindow::onGameTick()
{
    qint64 deltaMs = m_frameTimer.elapsed();
    m_frameTimer.restart();

    m_engine->update(deltaMs);
}

void MainWindow::onGameOver()
{
    const ProgressUpdateResult result = m_progress.recordGameResult(
        m_engine->score(),
        m_engine->level(),
        m_engine->totalLines(),
        modeLabel(m_engine->mode()));
    GameOverDialog dialog(m_engine, result, this);

    connect(&dialog, &GameOverDialog::restartRequested, this, [this]() {
        configureSkillsForMode(m_engine->mode(), m_progress.snapshot().selectedLoadoutSkillIds);
        m_engine->restart();
        m_frameTimer.start();
        showGameUI();
        startGameTimer();
        m_boardWidget->setFocus();
    });
    connect(&dialog, &GameOverDialog::menuRequested, this, &MainWindow::showMenu);

    dialog.exec();
}

void MainWindow::showMenu()
{
    showMenuUI();
}

void MainWindow::configureSkillsForMode(GameMode mode, const QStringList& selectedLoadoutSkillIds)
{
    const ProgressSnapshot snapshot = m_progress.snapshot();
    m_skillMgr->setUnlockedSkills(snapshot.unlockedSkillIds);

    if (mode != GameMode::InfiniteChallenge) {
        m_skillMgr->setLoadoutSkills({});
        m_skillMgr->reset();
        return;
    }

    const QStringList loadout = selectedLoadoutSkillIds.isEmpty()
        ? snapshot.selectedLoadoutSkillIds
        : selectedLoadoutSkillIds;
    m_skillMgr->setLoadoutSkills(loadout);
    m_skillMgr->setDefaultSlot(0);
    m_skillMgr->reset();
}

QString MainWindow::modeLabel(GameMode mode) const
{
    switch (mode) {
        case GameMode::Classic: return QStringLiteral("Classic");
        case GameMode::InfiniteChallenge: return QStringLiteral("Challenge");
        case GameMode::Battle: return QStringLiteral("Battle");
    }
    return QStringLiteral("Unknown");
}

void MainWindow::showGameUI()
{
    m_stack->setCurrentIndex(1);
    m_boardWidget->setFocus();
}

void MainWindow::showMenuUI()
{
    stopGameTimer();
    m_menuPage->setProgressSnapshot(m_progress.snapshot());
    m_stack->setCurrentIndex(0);
}

void MainWindow::startGameTimer()
{
    m_gameTimer->start();
}

void MainWindow::stopGameTimer()
{
    m_gameTimer->stop();
}
