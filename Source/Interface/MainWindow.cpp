#include "MainWindow.hpp"

#include <QVariant>
#include <QScrollArea>

MainWindow::MainWindow(const QGLFormat &format)
{
    // Create main renderer
    rendererWidget_ = new RendererWidget(format);
    
    // Create groups
    statsGroupBox_ = new QGroupBox("Stats");
    featureToggles_ = new QGroupBox("Shader Features");
    shadowMethodRadios_ = new QGroupBox("Shadow Methods");
    overlayRadios_ = new QGroupBox("Debug Overlay");
    shadowResolutionRadios_ = new QGroupBox("Shadow Map Resolution");
    shadowCascadesRadios_ = new QGroupBox("Shadow Cascades");
    voxelPCFFilterSizeRadios_ = new QGroupBox("Voxel PCF");
    
    // Use a vertical layout for all groups
    statsGroupBox_->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
    featureToggles_->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
    shadowMethodRadios_->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
    overlayRadios_->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
    shadowResolutionRadios_->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
    shadowCascadesRadios_->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
    voxelPCFFilterSizeRadios_->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
    
    // Create stats widgets
    treeResolutionLabel_ = createStatsLabel();
    treeCompletedTilesLabel_ = createStatsLabel();
    treeTotalTilesLabel_ = createStatsLabel();
    originalSizeLabel_ = createStatsLabel();
    treeSizeLabel_ = createStatsLabel();
    
    // Create shading feature toggles
    createFeatureToggle(SF_Texture, "Diffuse Textures");
    createFeatureToggle(SF_Specular, "Specular Highlights");
    createFeatureToggle(SF_NormalMap, "Normal Mapping");
    createFeatureToggle(SF_Cutout, "Cutout Transparency");
    createFeatureToggle(SF_Fog, "Fog");
    
    // Create shadow method toggles
    createShadowMethodRadio(SMM_ShadowMap, "Shadow Mapping");    createShadowMethodRadio(SMM_VoxelTree, "Voxel Tree");
    createShadowMethodRadio(SMM_Combined, "Combined")->setChecked(true); // Default
;

    // Create overlay radios
    // Must match RendererWidget::createOverlays()
    createOverlayRadio(-1, "No Overlay")->setChecked(true); // Default = No Overlay
    createOverlayRadio(0, "Shadow Map");
    createOverlayRadio(1, "Scene Depth");
    createOverlayRadio(2, "Shadow Mask");
    createOverlayRadio(3, "Cascade Splits");
    createOverlayRadio(4, "Voxel Tree Depth");
    
    // Create shadow resolution radios
    createShadowResolutionRadio(512);
    createShadowResolutionRadio(1024);
    createShadowResolutionRadio(2048);
    createShadowResolutionRadio(4096)->setChecked(true); // Default = 4096
    
    // Create shadow cascades radios
    createShadowCascadesRadio(1);
    createShadowCascadesRadio(2)->setChecked(true); // Default = 2
    createShadowCascadesRadio(3);
    createShadowCascadesRadio(4);
    
    // Create voxel pcf kernel size radios
    createVoxelPCFFilterSizeRadio(0);
    createVoxelPCFFilterSizeRadio(9)->setChecked(true); // Default = 9x9 PCF
    createVoxelPCFFilterSizeRadio(17);
    
    // Add widgets to side panel
    QBoxLayout* sidePanelLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    sidePanelLayout->addWidget(statsGroupBox_);
    sidePanelLayout->addWidget(featureToggles_);
    sidePanelLayout->addWidget(shadowMethodRadios_);
    sidePanelLayout->addWidget(overlayRadios_);
    sidePanelLayout->addWidget(shadowResolutionRadios_);
    sidePanelLayout->addWidget(shadowCascadesRadios_);
    sidePanelLayout->addWidget(voxelPCFFilterSizeRadios_);
    sidePanelLayout->setSpacing(20);
    sidePanelLayout->addStretch();
    
    QWidget* sidePanel = new QWidget();
    sidePanel->setMaximumWidth(200);
    sidePanel->setLayout(sidePanelLayout);
    
    // Create the scroll area for the side panel
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setMaximumWidth(220);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    scrollArea->setWidget(sidePanel);
    scrollArea->setWidgetResizable(false);
    scrollArea->setFrameShape(QFrame::NoFrame);
    
    // Add widgets to main layout
    QBoxLayout* mainLayout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    mainLayout->addWidget(rendererWidget_);
    mainLayout->addWidget(scrollArea);
}

QLabel* MainWindow::createStatsLabel()
{
    QLabel* label = new QLabel();
    statsGroupBox_->layout()->addWidget(label);
    return label;
}

QCheckBox* MainWindow::createFeatureToggle(ShaderFeature feature, const char* label)
{
    // Create the check box
    QCheckBox* checkBox = new QCheckBox(label);
    checkBox->setProperty("featureID", (int)feature);
    checkBox->setChecked(true);
    
    // Add to the feature toggles group
    featureToggles_->layout()->addWidget(checkBox);
    
    return checkBox;
}

QRadioButton* MainWindow::createShadowMethodRadio(ShadowMaskMethod method, const char* label)
{
    QRadioButton* radio = new QRadioButton(label);
    radio->setProperty("method", (int)method);
    
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

QRadioButton* MainWindow::createShadowCascadesRadio(int cascades)
{
    // Create the radio button
    QString label = QString("%1 Cascades").arg(cascades == 1 ? "No" : QString::number(cascades));
    QRadioButton* radio = new QRadioButton(label);
    radio->setProperty("cascades", cascades);
    
    shadowCascadesRadios_->layout()->addWidget(radio);
    
    return radio;
}

QRadioButton* MainWindow::createVoxelPCFFilterSizeRadio(int kernelSize)
{
    // Create the radio button
    QString label = kernelSize == 0 ? "No PCF" : QString("%1 x %1").arg(kernelSize);
    QRadioButton* radio = new QRadioButton(label);
    radio->setProperty("kernelSize", kernelSize);
    
    voxelPCFFilterSizeRadios_->layout()->addWidget(radio);
    
    return radio;
}
