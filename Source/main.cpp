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

int getTreeResolution(int argc, char* argv[])
{
    // Look for a resolution flag
    if(flagSet("2k", argc, argv)) return 2048;
    if(flagSet("4k", argc, argv)) return 4096;
    if(flagSet("8k", argc, argv)) return 8192;
    if(flagSet("16k", argc, argv)) return 16384;
    if(flagSet("32k", argc, argv)) return 32768;
    if(flagSet("64k", argc, argv)) return 65536;
    if(flagSet("128k", argc, argv)) return 131072;
    if(flagSet("256k", argc, argv)) return 262144;
    if(flagSet("512k", argc, argv)) return 524288;
    
    // No flag set, use 32K as the default
    return 32768;
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    
    // Specify OpenGL 4.0 Core Profile
    QGLFormat format = QGLFormat::defaultFormat();
    format.setVersion(4, 0);
    format.setProfile(QGLFormat::CoreProfile);
    
    // Create the window and controller
    MainWindow* window = new MainWindow(format, getTreeResolution(argc, argv));
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
