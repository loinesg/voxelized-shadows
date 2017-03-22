#pragma once

#include <vector>

#include <QWidget>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QRadioButton>

using namespace std;

#include "RendererWidget.hpp"
#include "FeatureToggle.hpp"
#include "OverlayToggle.hpp"

class MainWindow : public QWidget
{
public:
    MainWindow(const QGLFormat &format);

    RendererWidget* rendererWidget() const { return rendererWidget_; }

    // Shader feature check boxes
    const QObjectList featureToggles() const { return featureToggles_->children(); }
    
    // Shadow render method radio buttons
    QRadioButton* shadowMapMethodRadio() const { return shadowMapMethodRadio_; }
    QRadioButton* voxelTreeMethodRadio() const { return voxelTreeMethodRadio_; }
    
    // Debug overlay radio buttons
    const QObjectList overlayRadios() const { return overlayRadios_->children(); }
    
    // Shadow map resolution radio buttons
    QRadioButton* shadowResolution512Radio() const { return shadowResolution512Radio_; }
    QRadioButton* shadowResolution1024Radio() const { return shadowResolution1024Radio_; }
    QRadioButton* shadowResolution2048Radio() const { return shadowResolution2048Radio_; }
    QRadioButton* shadowResolution4096Radio() const { return shadowResolution4096Radio_; }
    
    // Shadow cascade count radio buttons
    QRadioButton* shadowCascades1() const { return shadowCascades1_; }
    QRadioButton* shadowCascades2() const { return shadowCascades2_; }
    QRadioButton* shadowCascades3() const { return shadowCascades3_; }
    QRadioButton* shadowCascades4() const { return shadowCascades4_; }
    
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
    
    // Shadow resolution radio buttons
    QGroupBox* shadowResolutionRadios_;
    QRadioButton* shadowResolution512Radio_;
    QRadioButton* shadowResolution1024Radio_;
    QRadioButton* shadowResolution2048Radio_;
    QRadioButton* shadowResolution4096Radio_;
    
    // Shadow cascades count radio buttons
    QGroupBox* shadowCascadesRadios_;
    QRadioButton* shadowCascades1_;
    QRadioButton* shadowCascades2_;
    QRadioButton* shadowCascades3_;
    QRadioButton* shadowCascades4_;
    
    void createFeatureToggles();
    void createShadowMethodRadios();
    void createOverlayRadios();
    void createShadowResolutionRadios();
    void createShadowCascadesRadios();
    
    QRadioButton* createShadowMethodRadio(const char* label);
    QRadioButton* createShadowResolutionRadio(const char* label);
    QRadioButton* createShadowCascadesRadio(const char* label);
};
