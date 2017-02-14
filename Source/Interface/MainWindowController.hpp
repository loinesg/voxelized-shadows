#pragma once

#include "MainWindow.hpp"

class MainWindowController : public QObject
{
    Q_OBJECT

public:
    MainWindowController(MainWindow* window);
    ~MainWindowController();

public slots:

private:
    MainWindow* window_;
};
