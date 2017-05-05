#include <QApplication>

#include <string>

#include "MainWindow.hpp"
#include "MainWindowController.hpp"

bool flagSet(std::string flag, int argc, char* argv[])
{
    for(int i = 0; i < argc; ++i)
    {
        // Check if the flag exists
        std::string actualValue(argv[i]);
        if(actualValue == flag)
        {
            return true;
        }
    }
    
    // No flag set.
    return false;
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    
    // Specify OpenGL 4.0 Core Profile
    QGLFormat format = QGLFormat::defaultFormat();
    format.setVersion(4, 0);
    format.setProfile(QGLFormat::CoreProfile);
    
    // Create the window and controller
    MainWindow* window = new MainWindow(format);
    MainWindowController* controller = new MainWindowController(window);

    // Pass all events to the controller
    app.installEventFilter(controller);

    // Show the window
    window->resize(1350, 850);
    window->setWindowTitle("Shadow Rendering");
    window->show();
    
    // Precompute the voxel tree, if specified
    if(flagSet("-precompute", argc, argv))
    {
        window->rendererWidget()->precomputeTree();
    }

    return app.exec();
}
