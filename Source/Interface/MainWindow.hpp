#pragma once

#include <QWidget>
#include <QtWidgets/QBoxLayout>

#include "RendererWidget.hpp"

class MainWindow : public QWidget
{
public:
    MainWindow();
    ~MainWindow();

    RendererWidget* rendererWidget() const { return rendererWidget_; }

private:
    RendererWidget* rendererWidget_;
    QBoxLayout* layout_;
};
