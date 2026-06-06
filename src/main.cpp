#include <QApplication>

#include "ui/MainWindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("CodexLocal");
    app.setApplicationName("Tetris Skill Battle");
    app.setApplicationVersion("1.0.0");

    MainWindow window;
    window.setWindowTitle(QStringLiteral("俄罗斯方块：技能大作战"));
    window.resize(800, 650);
    window.show();

    return app.exec();
}
