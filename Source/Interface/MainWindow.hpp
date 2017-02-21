#pragma once

#include <QWidget>
#include <QtWidgets/QBoxLayout>

#include "RendererWidget.hpp"

class MainWindow : public QWidget
{
public:
    MainWindow(const QGLFormat &format);
    ~MainWindow();

    RendererWidget* rendererWidget() const { return rendererWidget_; }

private:
    RendererWidget* rendererWidget_;
    QBoxLayout* layout_;
};
