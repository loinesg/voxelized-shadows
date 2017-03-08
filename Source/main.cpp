#include <QApplication>
#include <QGLWidget>

#include "MainWindow.hpp"
#include "MainWindowController.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    
    // Specify OpenGL 3.3 Core Profile
    QGLFormat format = QGLFormat::defaultFormat();
    format.setVersion(3, 3);
    format.setProfile(QGLFormat::CoreProfile);
    
    // Create the window and controller
    MainWindow* window = new MainWindow(format);
    MainWindowController* controller = new MainWindowController(window);

    // Pass all events to the controller
    app.installEventFilter(controller);

    // Show the window
    window->show();
    window->setWindowTitle("Shadow Rendering");
    window->resize(1200, 900);

    return app.exec();
}
