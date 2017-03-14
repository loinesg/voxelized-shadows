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
    
    // Shadow render method radio buttons
    QRadioButton* shadowMapMethodRadio() const { return shadowMapMethodRadio_; }
    QRadioButton* voxelTreeMethodRadio() const { return voxelTreeMethodRadio_; }
    
    // Debug overlay radio buttons
    QRadioButton* noOverlayRadio() const { return noOverlayRadio_; }
    QRadioButton* shadowMapOverlayRadio() const { return shadowMapOverlayRadio_; }
    QRadioButton* sceneDepthOverlayRadio() const { return sceneDepthOverlayRadio_; }
    QRadioButton* shadowMaskOverlayRadio() const { return shadowMaskOverlayRadio_; }
    QRadioButton* cascadeSplitsOverlayRadio() const { return cascadeSplitsOverlayRadio_; }
    QRadioButton* voxelTreeDepthOverlayRadio() const { return voxelTreeDepthOverlayRadio_; }
    
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
    QCheckBox* textureToggle_;
    QCheckBox* specularToggle_;
    QCheckBox* normalMapToggle_;
    QCheckBox* cutoutToggle_;
    
    // Shadow method radio buttons
    QGroupBox* shadowMethodRadios_;
    QRadioButton* shadowMapMethodRadio_;
    QRadioButton* voxelTreeMethodRadio_;
    
    // Debug texture radio buttons
    QGroupBox* overlayRadios_;
    QRadioButton* noOverlayRadio_;
    QRadioButton* shadowMapOverlayRadio_;
    QRadioButton* sceneDepthOverlayRadio_;
    QRadioButton* shadowMaskOverlayRadio_;
    QRadioButton* cascadeSplitsOverlayRadio_;
    QRadioButton* voxelTreeDepthOverlayRadio_;
    
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
    
    QCheckBox* createFeatureToggle(ShaderFeature feature, const char* label, bool on);
    QRadioButton* createShadowMethodRadio(const char* label);
    QRadioButton* createOverlayRadio(const char* label);
    QRadioButton* createShadowResolutionRadio(const char* label);
    QRadioButton* createShadowCascadesRadio(const char* label);
};
