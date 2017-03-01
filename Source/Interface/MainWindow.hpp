#pragma once

#include <vector>

#include <QWidget>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>

using namespace std;

#include "RendererWidget.hpp"

class MainWindow : public QWidget
{
public:
    MainWindow(const QGLFormat &format);

    RendererWidget* rendererWidget() const { return rendererWidget_; }

    // Shader features
    QCheckBox* textureToggle() const { return textureToggle_; }
    QCheckBox* specularToggle() const { return specularToggle_; }
    QCheckBox* normalMapToggle() const { return normalMapToggle_; }
    QCheckBox* cutoutToggle() const { return cutoutToggle_; }
    
private:
    // Main renderer widget
    RendererWidget* rendererWidget_;
    
    // Shader feature toggles
    QGroupBox* featureToggles_;
    QCheckBox* textureToggle_;
    QCheckBox* specularToggle_;
    QCheckBox* normalMapToggle_;
    QCheckBox* cutoutToggle_;
    
    void createFeatureToggles();
    QCheckBox* createFeatureToggle(ShaderFeature feature, const char* label, bool on);
};
