#include "MainWindow.hpp"

MainWindow::MainWindow(const QGLFormat &format)
{
    // Create main renderer
    rendererWidget_ = new RendererWidget(format);
    
    // Create side panel widgets
    featureToggles_ = new QGroupBox("Shader Features");
    featureToggles_->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
    createFeatureToggles();
    
    // Add widgets to side panel
    QBoxLayout* sidePanelLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    sidePanelLayout->addWidget(featureToggles_);
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

QCheckBox* MainWindow::createFeatureToggle(ShaderFeature feature, const char* label, bool on)
{
    QCheckBox* checkBox = new QCheckBox(label);
    checkBox->setChecked(on);
    
    featureToggles_->layout()->addWidget(checkBox);
    
    return checkBox;
}
