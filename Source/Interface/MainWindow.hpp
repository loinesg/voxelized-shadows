#pragma once

#include <QWidget>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QLabel>

#include "RendererWidget.hpp"

class MainWindow : public QWidget
{
public:
    MainWindow(const QGLFormat &format);

    RendererWidget* rendererWidget() const { return rendererWidget_; }

    // Stats widgets
    QLabel* treeResolutionLabel() const { return treeResolutionLabel_; }
    QLabel* treeCompletedTilesLabel() const { return treeCompletedTilesLabel_; }
    QLabel* treeTotalTilesLabel() const { return treeTotalTilesLabel_; }
    QLabel* originalSizeLabel() const { return originalSizeLabel_; }
    QLabel* treeSizeLabel() const { return treeSizeLabel_; }
    
    // Render setting widget lists
    QObjectList shaderFeatureToggles() { return featureToggles_->children(); }
    QObjectList shadowMethodRadios() { return shadowMethodRadios_->children(); }
    QObjectList overlayRadios() { return overlayRadios_->children(); }
    QObjectList shadowResolutionRadios() { return shadowResolutionRadios_->children(); }
    QObjectList shadowCascadesRadios() { return shadowCascadesRadios_->children(); }
    
private:
    // Main renderer widget
    RendererWidget* rendererWidget_;
    
    // Stats labels
    QGroupBox* statsGroupBox_;
    QLabel* treeResolutionLabel_;
    QLabel* treeCompletedTilesLabel_;
    QLabel* treeTotalTilesLabel_;
    QLabel* originalSizeLabel_;
    QLabel* treeSizeLabel_;
    
    // Render setting widget groups
    QGroupBox* featureToggles_;
    QGroupBox* shadowMethodRadios_;
    QGroupBox* overlayRadios_;
    QGroupBox* shadowResolutionRadios_;
    QGroupBox* shadowCascadesRadios_;
    
    QLabel* createStatsLabel();
    QCheckBox* createFeatureToggle(ShaderFeature feature, const char* label);
    QRadioButton* createShadowMethodRadio(ShadowMaskMethod method, const char* label);
    QRadioButton* createOverlayRadio(int index, const char* label);
    QRadioButton* createShadowResolutionRadio(int resolution);
    QRadioButton* createShadowCascadesRadio(int cascades);
};
