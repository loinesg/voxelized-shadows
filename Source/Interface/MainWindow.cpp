#include "MainWindow.hpp"

MainWindow::MainWindow(const QGLFormat &format)
{
    // Create main renderer
    rendererWidget_ = new RendererWidget(format);
    
    // Create shader feature check boxes
    featureToggles_ = new QGroupBox("Shader Features");
    featureToggles_->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
    createFeatureToggles();
    
    // Create debug overlay radio buttons
    overlayRadios_ = new QGroupBox("Debug Overlay");
    overlayRadios_->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
    createOverlayRadios();
    
    // Create shadow map resolution radio buttons
    shadowResolutionRadios_ = new QGroupBox("Shadow Map Resolution");
    shadowResolutionRadios_->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
    createShadowResolutionRadios();
    
    // Add widgets to side panel
    QBoxLayout* sidePanelLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    sidePanelLayout->addWidget(featureToggles_);
    sidePanelLayout->addSpacing(20);
    sidePanelLayout->addWidget(overlayRadios_);
    sidePanelLayout->addSpacing(20);
    sidePanelLayout->addWidget(shadowResolutionRadios_);
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
    textureToggle_ = createFeatureToggle(SF_Texture, "Diffuse Textures", true);
    specularToggle_ = createFeatureToggle(SF_Specular, "Specular Highlights", true);
    normalMapToggle_ = createFeatureToggle(SF_NormalMap, "Normal Mapping", true);
    cutoutToggle_ = createFeatureToggle(SF_Cutout, "Cutout Transparency", true);
}

void MainWindow::createOverlayRadios()
{
    noOverlayRadio_ = createOverlayRadio("No Overlay");
    shadowMapOverlayRadio_ = createOverlayRadio("Shadow Map");
    sceneDepthOverlayRadio_ = createOverlayRadio("Scene Depth");
    shadowMaskOverlayRadio_ = createOverlayRadio("Shadow Mask");
    
    // Default = No Overlay
    noOverlayRadio_->setChecked(true);
}

void MainWindow::createShadowResolutionRadios()
{
    shadowResolution512Radio_ = createShadowResolutionRadio("512x512");
    shadowResolution1024Radio_ = createShadowResolutionRadio("1024x1024");
    shadowResolution2048Radio_ = createShadowResolutionRadio("2048x2048");
    shadowResolution4096Radio_ = createShadowResolutionRadio("4096x4096");
    
    // Default = 4096
    shadowResolution4096Radio_->setChecked(true);
}

QCheckBox* MainWindow::createFeatureToggle(ShaderFeature feature, const char* label, bool on)
{
    QCheckBox* checkBox = new QCheckBox(label);
    checkBox->setChecked(on);
    
    featureToggles_->layout()->addWidget(checkBox);
    
    return checkBox;
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
