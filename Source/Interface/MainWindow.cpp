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
    // Create group box
    featureToggles_ = new QGroupBox("Shader Features");

    // Create a layout to contain the check boxes
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
    featureToggles_->setLayout(layout);
    
    // Create check boxes
    layout->addWidget(new FeatureToggle(SF_Texture, "Diffuse Textures"));
    layout->addWidget(new FeatureToggle(SF_Specular, "Specular Highlights"));
    layout->addWidget(new FeatureToggle(SF_NormalMap, "Normal Mapping"));
    layout->addWidget(new FeatureToggle(SF_Cutout, "Cutout Transparency"));
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
    
    // Create a layout for the radio buttons
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
    overlayRadios_->setLayout(layout);
    
    // One radio per overlay
    // Indexes must match RendererWidget::createOverlays()
    layout->addWidget(new OverlayToggle(-1, "No Overlay"));
    layout->addWidget(new OverlayToggle(0, "Shadow Map"));
    layout->addWidget(new OverlayToggle(1, "Scene Depth"));
    layout->addWidget(new OverlayToggle(2, "Shadow Mask"));
    layout->addWidget(new OverlayToggle(3, "Cascade Splits"));
    layout->addWidget(new OverlayToggle(4, "Projected Shadow Map"));
    layout->addWidget(new OverlayToggle(5, "Voxel Tree Depth"));
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

QRadioButton* MainWindow::createShadowMethodRadio(const char* label)
{
    QRadioButton* radio = new QRadioButton(label);
    shadowMethodRadios_->layout()->addWidget(radio);
    
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
