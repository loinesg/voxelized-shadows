#include "MainWindow.hpp"

MainWindow::MainWindow(const QGLFormat &format)
{
    // Create widgets
    rendererWidget_ = new RendererWidget(format);

    // Add widgets to layout
    layout_ = new QBoxLayout(QBoxLayout::TopToBottom, this);
    layout_->addWidget(rendererWidget_);
}

MainWindow::~MainWindow()
{
    delete rendererWidget_;
    delete layout_;
}
