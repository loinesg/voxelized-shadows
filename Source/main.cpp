#include <QApplication>
#include <QGLWidget>

#include "MainWindow.hpp"
#include "MainWindowController.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MainWindow* window = new MainWindow();
    MainWindowController* controller = new MainWindowController(window);

    window->show();
    window->setWindowTitle("Shadow Rendering");
    window->resize(800, 900);

    return app.exec();
}
