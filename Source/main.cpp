#include <QApplication>
#include <QGLWidget>

#include "MainWindow.hpp"
#include "MainWindowController.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    
    QGLFormat format = QGLFormat::defaultFormat();
    format.setVersion(3, 3);
    format.setProfile(QGLFormat::CoreProfile);
    
    MainWindow* window = new MainWindow(format);
    MainWindowController* controller = new MainWindowController(window);

    window->show();
    window->setWindowTitle("Shadow Rendering");
    window->resize(800, 900);

    return app.exec();
}
