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

    // Shading setting widgets
    QObjectList shaderFeatureToggles() { return featureToggles_->children(); }
    
    // Debug overlay radio buttons
    QObjectList overlayRadios() { return overlayRadios_->children(); }
    
    // Shadow mapping setting widgets
    QObjectList shadowResolutionRadios() { return shadowResolutionRadios_->children(); }
    QObjectList shadowCascadesRadios() { return shadowCascadesRadios_->children(); }
    
    // Shadow render method radio buttons
    QRadioButton* shadowMapMethodRadio() const { return shadowMapMethodRadio_; }
    QRadioButton* voxelTreeMethodRadio() const { return voxelTreeMethodRadio_; }
    
private:
    // Main renderer widget
    RendererWidget* rendererWidget_;
    
    // Shader feature toggles
    QGroupBox* featureToggles_;
    
    // Shadow method radio buttons
    QGroupBox* shadowMethodRadios_;
    QRadioButton* shadowMapMethodRadio_;
    QRadioButton* voxelTreeMethodRadio_;
    
    // Debug overlay radio buttons
    QGroupBox* overlayRadios_;
    
    // Shadow map radio button groups
    QGroupBox* shadowResolutionRadios_;
    QGroupBox* shadowCascadesRadios_;
    
    void createFeatureToggles();
    void createShadowMethodRadios();
    void createOverlayRadios();
    void createShadowResolutionRadios();
    void createShadowCascadesRadios();
    
    QCheckBox* createFeatureToggle(ShaderFeature feature, const char* label, bool on);
    QRadioButton* createShadowMethodRadio(const char* label);
    QRadioButton* createOverlayRadio(int index, const char* label);
    QRadioButton* createShadowResolutionRadio(int resolution);
    QRadioButton* createShadowCascadesRadio(int cascades);
};
