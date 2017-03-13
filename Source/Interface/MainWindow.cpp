#include "MainWindow.hpp"

#include <cstdio>

MainWindow::MainWindow(const QGLFormat &format)
{
    // Create main renderer
    rendererWidget_ = new RendererWidget(format);
    
    // Create check boxes and radios
    createFeatureToggles();
    createShadowMethodRadios();
    createOverlayRadios();
    createShadowResolutionRadios();
    createShadowCascadesRadios();
    
    // Add widgets to side panel
    QBoxLayout* sidePanelLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    sidePanelLayout->addWidget(featureToggles_);
    sidePanelLayout->addSpacing(20);
    sidePanelLayout->addWidget(shadowMethodRadios_);
    sidePanelLayout->addSpacing(20);
    sidePanelLayout->addWidget(overlayRadios_);
    sidePanelLayout->addSpacing(20);
    sidePanelLayout->addWidget(shadowResolutionRadios_);
    sidePanelLayout->addSpacing(20);
    sidePanelLayout->addWidget(shadowCascadesRadios_);
    sidePanelLayout->addStretch();
    
    QWidget* sidePanel = new QWidget();
    sidePanel->setMaximumWidth(200);
    sidePanel->setLayout(sidePanelLayout);
    
    // Add widgets to main layout
    QBoxLayout* mainLayout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    mainLayout->addWidget(rendererWidget_);
    mainLayout->addWidget(sidePanel);
}

void MainWindow::createFeatureToggles()
{
    featureToggles_ = new QGroupBox("Shader Features");
    featureToggles_->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
    
    textureToggle_ = createFeatureToggle(SF_Texture, "Diffuse Textures", true);
    specularToggle_ = createFeatureToggle(SF_Specular, "Specular Highlights", true);
    normalMapToggle_ = createFeatureToggle(SF_NormalMap, "Normal Mapping", true);
    cutoutToggle_ = createFeatureToggle(SF_Cutout, "Cutout Transparency", true);
}

void MainWindow::createShadowMethodRadios()
{
    // Create group box
    shadowMethodRadios_ = new QGroupBox("Shadow Methods");
    shadowMethodRadios_->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
    
    // One radio per method
    shadowMapMethodRadio_ = createShadowMethodRadio("Shadow Mapping");
    voxelTreeMethodRadio_ = createShadowMethodRadio("Voxel Tree");
    
    // Default = Shadow Mapping
    shadowMapMethodRadio_->setChecked(true);
}

void MainWindow::createOverlayRadios()
{
    // Create group box
    overlayRadios_ = new QGroupBox("Debug Overlay");
    overlayRadios_->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
    
    // One radio per overlay
    // Must match RendererWidget::createOverlays()
    noOverlayRadio_ = createOverlayRadio("No Overlay");
    shadowMapOverlayRadio_ = createOverlayRadio("Shadow Map");
    sceneDepthOverlayRadio_ = createOverlayRadio("Scene Depth");
    shadowMaskOverlayRadio_ = createOverlayRadio("Shadow Mask");
    cascadeSplitsOverlayRadio_ = createOverlayRadio("Cascade Splits");
    
    // Default = No Overlay
    noOverlayRadio_->setChecked(true);
}

void MainWindow::createShadowResolutionRadios()
{
    shadowResolutionRadios_ = new QGroupBox("Shadow Map Resolution");
    shadowResolutionRadios_->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
    
    shadowResolution512Radio_ = createShadowResolutionRadio("512x512");
    shadowResolution1024Radio_ = createShadowResolutionRadio("1024x1024");
    shadowResolution2048Radio_ = createShadowResolutionRadio("2048x2048");
    shadowResolution4096Radio_ = createShadowResolutionRadio("4096x4096");
    
    // Default = 4096
    shadowResolution4096Radio_->setChecked(true);
}

void MainWindow::createShadowCascadesRadios()
{
    shadowCascadesRadios_ = new QGroupBox("Shadow Cascades");
    shadowCascadesRadios_->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
    
    shadowCascades1_ = createShadowCascadesRadio("No Cascades");
    shadowCascades2_ = createShadowCascadesRadio("2 Cascades");
    shadowCascades3_ = createShadowCascadesRadio("3 Cascades");
    shadowCascades4_ = createShadowCascadesRadio("4 Cascades");
    
    // Default = 2
    shadowCascades2_->setChecked(true);
}

QCheckBox* MainWindow::createFeatureToggle(ShaderFeature feature, const char* label, bool on)
{
    QCheckBox* checkBox = new QCheckBox(label);
    checkBox->setChecked(on);
    
    featureToggles_->layout()->addWidget(checkBox);
    
    return checkBox;
}

QRadioButton* MainWindow::createShadowMethodRadio(const char* label)
{
    QRadioButton* radio = new QRadioButton(label);
    shadowMethodRadios_->layout()->addWidget(radio);
    
    return radio;
}

QRadioButton* MainWindow::createOverlayRadio(const char* label)
{
    QRadioButton* radio = new QRadioButton(label);
    overlayRadios_->layout()->addWidget(radio);
    
    return radio;
}

QRadioButton* MainWindow::createShadowResolutionRadio(const char* label)
{
    QRadioButton* radio = new QRadioButton(label);
    shadowResolutionRadios_->layout()->addWidget(radio);
    
    return radio;
}

QRadioButton* MainWindow::createShadowCascadesRadio(const char* label)
{
    QRadioButton* radio = new QRadioButton(label);
    shadowCascadesRadios_->layout()->addWidget(radio);
    
    return radio;
}
