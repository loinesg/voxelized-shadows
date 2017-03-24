#include "MainWindow.hpp"

#include <cstdio>
#include <QVariant>

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
    sidePanelLayout->addWidget(shadowMethodRadios_);
    sidePanelLayout->addWidget(overlayRadios_);
    sidePanelLayout->addWidget(shadowResolutionRadios_);
    sidePanelLayout->addWidget(shadowCascadesRadios_);
    sidePanelLayout->setSpacing(20);
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
    // Create the group box
    featureToggles_ = new QGroupBox("Shader Features");
    featureToggles_->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
    
    // Create the check boxes
    createFeatureToggle(SF_Texture, "Diffuse Textures", true);
    createFeatureToggle(SF_Specular, "Specular Highlights", true);
    createFeatureToggle(SF_NormalMap, "Normal Mapping", true);
    createFeatureToggle(SF_Cutout, "Cutout Transparency", true);
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
    createOverlayRadio(-1, "No Overlay")->setChecked(true); // Default = No Overlay
    createOverlayRadio(0, "Shadow Map");
    createOverlayRadio(1, "Scene Depth");
    createOverlayRadio(2, "Shadow Mask");
    createOverlayRadio(3, "Cascade Splits");
    createOverlayRadio(4, "Projected Shadow Map");
    createOverlayRadio(5, "Voxel Tree Depth");
}

void MainWindow::createShadowResolutionRadios()
{
    shadowResolutionRadios_ = new QGroupBox("Shadow Map Resolution");
    shadowResolutionRadios_->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
    
    createShadowResolutionRadio(512);
    createShadowResolutionRadio(1024);
    createShadowResolutionRadio(2048);
    createShadowResolutionRadio(4096)->setChecked(true); // Default = 4096
}

void MainWindow::createShadowCascadesRadios()
{
    shadowCascadesRadios_ = new QGroupBox("Shadow Cascades");
    shadowCascadesRadios_->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
    
    createShadowCascadesRadio(1);
    createShadowCascadesRadio(2)->setChecked(true); // Default = 2
    createShadowCascadesRadio(3);
    createShadowCascadesRadio(4);
}

QCheckBox* MainWindow::createFeatureToggle(ShaderFeature feature, const char* label, bool on)
{
    // Create the check box
    QCheckBox* checkBox = new QCheckBox(label);
    checkBox->setProperty("featureID", (int)feature);
    checkBox->setChecked(on);
    
    // Add to the feature toggles group
    featureToggles_->layout()->addWidget(checkBox);
    
    return checkBox;
}

QRadioButton* MainWindow::createShadowMethodRadio(const char* label)
{
    QRadioButton* radio = new QRadioButton(label);
    shadowMethodRadios_->layout()->addWidget(radio);
    
    return radio;
}

QRadioButton* MainWindow::createOverlayRadio(int index, const char* label)
{
    // Creat the radio button
    QRadioButton* radio = new QRadioButton(label);
    radio->setProperty("overlay", index);
    
    // Add to the overlay radios group
    overlayRadios_->layout()->addWidget(radio);
    
    return radio;
}

QRadioButton* MainWindow::createShadowResolutionRadio(int resolution)
{
    // Create the radio button
    QRadioButton* radio = new QRadioButton(QString("%1 x %1").arg(resolution));
    radio->setProperty("resolution", resolution);
    
    // Add to the shadow resolution radios group
    shadowResolutionRadios_->layout()->addWidget(radio);
    
    return radio;
}

QRadioButton* MainWindow::createShadowCascadesRadio(const int cascades)
{
    // Create the radio button
    QString label = QString("%1 Cascades").arg(cascades == 1 ? "No" : QString::number(cascades));
    QRadioButton* radio = new QRadioButton(label);
    radio->setProperty("cascades", cascades);
    
    shadowCascadesRadios_->layout()->addWidget(radio);
    
    return radio;
}
