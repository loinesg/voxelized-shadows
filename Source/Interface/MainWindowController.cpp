#include "MainWindowController.hpp"

MainWindowController::MainWindowController(MainWindow* window)
    : window_(window),
    inputManager_(),
    mouseDragging_(false),
    mousePosition_(Vector2(0, 0))
{
    // Shader feature toggle signals
    for(int i = 1; i < window_->shaderFeatureToggles().size(); ++i)
    {
        connect(window_->shaderFeatureToggles()[i], SIGNAL(stateChanged(int)), this, SLOT(shaderFeatureToggled()));
    }
    
    // Shadow method radio button signals
    for(int i = 1; i < window_->shadowMethodRadios().size(); ++i)
    {
        connect(window_->shadowMethodRadios()[i], SIGNAL(toggled(bool)), this, SLOT(shadowMethodToggled()));
    }
    
    // Debug overlay radio button signals
    for(int i = 1; i < window_->overlayRadios().size(); ++i)
    {
        connect(window_->overlayRadios()[i], SIGNAL(toggled(bool)), this, SLOT(overlayToggled()));
    }
    
    // Shadow map resolution radio button signals
    for(int i = 1; i < window_->shadowResolutionRadios().size(); ++i)
    {
        connect(window_->shadowResolutionRadios()[i], SIGNAL(toggled(bool)), SLOT(shadowResolutionToggled()));
    }
    
    // Shadow map cascades radio button signals
    for(int i = 1; i < window_->shadowCascadesRadios().size(); ++i)
    {
        connect(window_->shadowCascadesRadios()[i], SIGNAL(toggled(bool)), SLOT(shadowCascadesToggled()));
    }
    
    // Voxel PCF filter size radio button signals
    for(int i = 1; i < window_->voxelPCFFilterSizeRadios().size(); ++i)
    {
        connect(window_->voxelPCFFilterSizeRadios()[i], SIGNAL(toggled(bool)), SLOT(voxelPCFFilterSizeToggled()));
    }
}

bool MainWindowController::eventFilter(QObject* obj, QEvent* event)
{
    if(event->type() == QEvent::Paint)
    {
        update((1.0 / 60.0));
    }
    else if(event->type() == QEvent::MouseButtonPress && obj == window_->rendererWidget())
    {
        mousePressEvent(static_cast<QMouseEvent*>(event));
    }
    else if(event->type() == QEvent::MouseButtonRelease && obj == window_->rendererWidget())
    {
        mouseReleaseEvent(static_cast<QMouseEvent*>(event));
    }
    else if(event->type() == QEvent::MouseMove && obj == window_->rendererWidget())
    {
        mouseMoveEvent(static_cast<QMouseEvent*>(event));
    }
    
    // InputManager handles key press / release events
    if(event->type() == QEvent::KeyPress)
    {
        inputManager_.keyPressed((InputKey)static_cast<QKeyEvent*>(event)->key());
    }
    else if(event->type() == QEvent::KeyRelease)
    {
        inputManager_.keyReleased((InputKey)static_cast<QKeyEvent*>(event)->key());
    }
    
    // Unhandled events are passed back to Qt
    return QObject::eventFilter(obj, event);
}

void MainWindowController::shaderFeatureToggled()
{
    // The sender is a shader feature checkbox
    QCheckBox* sender = (QCheckBox*)QObject::sender();
    ShaderFeature feature = (ShaderFeature)sender->property("featureID").toInt();
    
    // Enable / disable based on check box state
    if(sender->isChecked())
    {
        window_->rendererWidget()->enableFeature(feature);
    }
    else
    {
        window_->rendererWidget()->disableFeature(feature);
    }
}

void MainWindowController::shadowMethodToggled()
{
    // The sender is a shadow method radio button
    QRadioButton* sender = (QRadioButton*)QObject::sender();
    ShadowMaskMethod method = (ShadowMaskMethod)sender->property("method").toInt();
    
    // Update the shadow method
    window_->rendererWidget()->setShadowRenderMethod(method);
}

void MainWindowController::overlayToggled()
{
    // The sender is a overlay radio button
    QRadioButton* radio = (QRadioButton*)QObject::sender();
    int overlay = radio->property("overlay").toInt();
    
    // Update the overlay index
    window_->rendererWidget()->setOverlay(overlay);

}

void MainWindowController::shadowResolutionToggled()
{
    // The sender is a shadow resolution radio button
    QRadioButton* radio = (QRadioButton*)QObject::sender();
    int resolution = radio->property("resolution").toInt();
    
    // Update the shadow map resolution
    window_->rendererWidget()->setShadowMapResolution(resolution);
}

void MainWindowController::shadowCascadesToggled()
{
    // The sender is a shadow cascades radio button
    QRadioButton* radio = (QRadioButton*)QObject::sender();
    int cascades = radio->property("cascades").toInt();
    
    // Update the shadow map resolution
    window_->rendererWidget()->setShadowMapCascades(cascades);
}

void MainWindowController::voxelPCFFilterSizeToggled()
{
    // The sender is a voxel pcf filter size radio button
    QRadioButton* radio = (QRadioButton*)QObject::sender();
    int kernelSize = radio->property("kernelSize").toInt();
    
    // Update the shadow map resolution
    window_->rendererWidget()->setVoxelPCFFilterSize(kernelSize);
}

void MainWindowController::update(float deltaTime)
{
    // Move the camera with user input
    applyCameraMovement(deltaTime);
    
    // Update the statistics ui
    updateStatsUI();
}

void MainWindowController::applyCameraMovement(float deltaTime)
{
    Camera* camera = window_->rendererWidget()->camera();

    // Calculate movement from inputs
    Vector3 movement;
    movement.x = inputManager_.getSidewaysMovement();
    movement.z = inputManager_.getForwardsMovement();
    movement.y = inputManager_.getVerticalMovement();
    movement = movement * inputManager_.getMovementSpeed();
    movement = camera->localToWorldVector(movement);
    
    // Apply movement to camera (frame rate independent)
    camera->translate(movement * deltaTime);
}

void MainWindowController::updateStatsUI()
{
    // Get the render resolution
    int resX = window_->rendererWidget()->resolutionX();
    int resY = window_->rendererWidget()->resolutionY();
    
    // Get the performance stats
    const RendererStats* stats = window_->rendererWidget()->stats();
    int frameRate = stats->currentFrameRate();
    int frameTime = stats->currentFrameTime();
    double shadowRenderingTime = stats->currentShadowRenderingTime();
    double shadowSamplingTime = stats->currentShadowSamplingTime();
    
    // Get the voxel tree stats
    const VoxelTree* tree = window_->rendererWidget()->voxelTree();
    int resolution = tree->resolution() / 1024;
    int totalTiles = tree->totalTiles();
    int completedTiles = tree->completedTiles();
    size_t originalSizeMB = tree->originalSizeMB();
    size_t treeSizeMB = tree->sizeMB();
    
    // Create the text for each label
    QString resolutionText = QString("%1 x %2").arg(resX).arg(resY);
    QString frameRateText = QString("Frame Rate: %1 FPS (%2 ms)").arg(frameRate).arg(frameTime);
    QString shadowRenderingText = QString("Shadow Rendering: %1 ms").arg(shadowRenderingTime, 0, 'f', 1);
    QString shadowSamplingText = QString("Shadow Sampling: %1 ms").arg(shadowSamplingTime, 0, 'f', 1);
    QString treeResolutionText = QString("Resolution: %1K x %1K").arg(resolution);
    QString tilesText = QString("Tiles: %1 / %2").arg(completedTiles).arg(totalTiles);
    QString originalSizeText = QString("Original Size: %1 MB").arg(originalSizeMB);
    QString treeSizeText = QString("Tree Size: %1 MB").arg(treeSizeMB);
    
    // Update the stats labels
    window_->resolutionLabel()->setText(resolutionText);
    window_->frameRateLabel()->setText(frameRateText);
    window_->shadowRenderingTimeLabel()->setText(shadowRenderingText);
    window_->shadowSamplingTimeLabel()->setText(shadowSamplingText);
    window_->treeResolutionLabel()->setText(treeResolutionText);
    window_->treeTilesLabel()->setText(tilesText);
    window_->originalSizeLabel()->setText(originalSizeText);
    window_->treeSizeLabel()->setText(treeSizeText);
}

void MainWindowController::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::MouseButton::LeftButton)
    {
        mouseDragging_ = true;
        mousePosition_ = Vector2(event->pos().x(), event->pos().y());
    }
}

void MainWindowController::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::MouseButton::LeftButton)
    {
        mouseDragging_ = false;
    }
}

void MainWindowController::mouseMoveEvent(QMouseEvent *event)
{
    if(mouseDragging_)
    {
        Camera* camera = window_->rendererWidget()->camera();
        
        // Determine mouse movement
        Vector2 oldPosition = mousePosition_;
        Vector2 newPosition = Vector2 (event->pos().x(), event->pos().y());
        Vector2 deltaPosition = newPosition - oldPosition;
        mousePosition_ = newPosition;
        
        // Apply horizontal rotation
        Quaternion horizontal = Quaternion::rotation(deltaPosition.x * 0.5, Vector3::up());

        // Calculate vertical rotation
        Vector3 upAxis = camera->up().vec3();
        Vector3 forwardAxis = camera->forward().vec3();
        Vector3 sidewaysAxis = Vector3::cross(upAxis, forwardAxis);
        Quaternion vertical = Quaternion::rotation(deltaPosition.y * 0.5, sidewaysAxis);
    
        // Apply rotation, vertical first
        camera->setRotation(horizontal * vertical * camera->rotation());
    }
}
