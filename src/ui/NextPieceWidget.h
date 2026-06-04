#ifndef NEXTPIECEWIDGET_H
#define NEXTPIECEWIDGET_H

#include <QWidget>

class GameEngine;

class NextPieceWidget : public QWidget {
    Q_OBJECT
public:
    explicit NextPieceWidget(GameEngine* engine, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    GameEngine* m_engine = nullptr;
};

#endif
