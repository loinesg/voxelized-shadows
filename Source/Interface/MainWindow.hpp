#pragma once

#include <vector>

#include <QWidget>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QRadioButton>

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
    
    // Debug overlay radio buttons
    QRadioButton* noOverlayRadio() const { return noOverlayRadio_; }
    QRadioButton* shadowMapOverlayRadio() const { return shadowMapOverlayRadio_; }
    QRadioButton* sceneDepthOverlayRadio() const { return sceneDepthOverlayRadio_; }
    QRadioButton* shadowMaskOverlayRadio() const { return shadowMaskOverlayRadio_; }
    
private:
    // Main renderer widget
    RendererWidget* rendererWidget_;
    
    // Shader feature toggles
    QGroupBox* featureToggles_;
    QCheckBox* textureToggle_;
    QCheckBox* specularToggle_;
    QCheckBox* normalMapToggle_;
    QCheckBox* cutoutToggle_;
    
    // Debug texture radio buttons
    QGroupBox* overlayRadios_;
    QRadioButton* noOverlayRadio_;
    QRadioButton* shadowMapOverlayRadio_;
    QRadioButton* sceneDepthOverlayRadio_;
    QRadioButton* shadowMaskOverlayRadio_;
    
    void createFeatureToggles();
    void createOverlayRadios();
    
    QCheckBox* createFeatureToggle(ShaderFeature feature, const char* label, bool on);
    QRadioButton* createOverlayRadio(const char* label);
};
