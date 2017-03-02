#include "MainWindowController.hpp"

MainWindowController::MainWindowController(MainWindow* window)
    : window_(window),
    inputManager_(),
    mouseDragging_(false),
    mousePosition_(Vector2(0, 0))
{
    window_->rendererWidget()->setFocus();
    window_->rendererWidget()->installEventFilter(this);
    
    // Shader feature toggle signals
    connect(window_->textureToggle(), SIGNAL(stateChanged(int)), this, SLOT(textureFeatureToggled(int)));
    connect(window_->specularToggle(), SIGNAL(stateChanged(int)), this, SLOT(specularFeatureToggled(int)));
    connect(window_->normalMapToggle(), SIGNAL(stateChanged(int)), this, SLOT(normalMapsFeatureToggled(int)));
    connect(window_->cutoutToggle(), SIGNAL(stateChanged(int)), this, SLOT(cutoutFeatureToggled(int)));
    
    // Debug texture toggle signals
    connect(window_->noOverlayRadio(), SIGNAL(toggled(bool)), this, SLOT(noOverlayToggled(int)));
    connect(window_->shadowMapOverlayRadio(), SIGNAL(toggled(bool)), this, SLOT(shadowMapOverlayToggled(bool)));
    connect(window_->sceneDepthOverlayRadio(), SIGNAL(toggled(bool)), this, SLOT(sceneDepthOverlayToggled(bool)));
    connect(window_->shadowMaskOverlayRadio(), SIGNAL(toggled(bool)), this, SLOT(shadowMaskOverlayToggled(bool)));
}

bool MainWindowController::eventFilter(QObject * obj, QEvent* event)
{
    if(event->type() == QEvent::Paint)
    {
        update((1.0 / 60.0));
    }
    else if(event->type() == QEvent::MouseButtonPress)
    {
        mousePressEvent(static_cast<QMouseEvent*>(event));
    }
    else if(event->type() == QEvent::MouseButtonRelease)
    {
        mouseReleaseEvent(static_cast<QMouseEvent*>(event));
    }
    else if(event->type() == QEvent::MouseMove)
    {
        mouseMoveEvent(static_cast<QMouseEvent*>(event));
    }
    else if(event->type() == QEvent::KeyPress)
    {
        keyPressEvent(static_cast<QKeyEvent*>(event));
        return true;
    }
    else if(event->type() == QEvent::KeyRelease)
    {
        keyReleaseEvent(static_cast<QKeyEvent*>(event));
        return true;
    }
    
    return QObject::eventFilter(obj, event);
}

void MainWindowController::textureFeatureToggled(int state)
{
    updateShaderFeature(SF_Texture, state);
}

void MainWindowController::specularFeatureToggled(int state)
{
    updateShaderFeature(SF_Specular, state);
}

void MainWindowController::normalMapsFeatureToggled(int state)
{
    updateShaderFeature(SF_NormalMap, state);
}

void MainWindowController::cutoutFeatureToggled(int state)
{
    updateShaderFeature(SF_Cutout, state);
}

void MainWindowController::noOverlayToggled(bool checked)
{
    if(checked)
    {
        window_->rendererWidget()->setOverlayTexture(NULL);
    }
}

void MainWindowController::shadowMapOverlayToggled(bool checked)
{
    RendererWidget* renderer = window_->rendererWidget();
    Texture* texture = renderer->shadowMap();
    
    if(checked)
    {
        renderer->setOverlayTexture(texture);
    }
}

void MainWindowController::sceneDepthOverlayToggled(bool checked)
{
    RendererWidget* renderer = window_->rendererWidget();
    Texture* texture = renderer->sceneDepthTexture();
    
    if(checked)
    {
        renderer->setOverlayTexture(texture);
    }
}

void MainWindowController::shadowMaskOverlayToggled(bool checked)
{
    RendererWidget* renderer = window_->rendererWidget();
    Texture* texture = renderer->shadowMask();
    
    if(checked)
    {
        renderer->setOverlayTexture(texture);
    }
}

void MainWindowController::update(float deltaTime)
{
    Camera* camera = window_->rendererWidget()->camera();

    // Calculate movement from inputs
    Vector3 movement;
    movement.x = inputManager_.getSidewaysMovement();
    movement.z = inputManager_.getForwardsMovement();
    movement.y = inputManager_.getVerticalMovement();
    movement = camera->localToWorldVector(movement);
    
    // Apply movement to camera (frame rate independent)
    float speed = (inputManager_.isKeyDown(IK_MoveFast)) ? 20.0 : 5.0;
    camera->translate(movement * speed * deltaTime);
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

void MainWindowController::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key::Key_W)
        inputManager_.keyPressed(IK_MoveForward);
    else if(event->key() == Qt::Key::Key_A)
        inputManager_.keyPressed(IK_MoveLeft);
    else if(event->key() == Qt::Key::Key_S)
        inputManager_.keyPressed(IK_MoveBackwards);
    else if(event->key() == Qt::Key::Key_D)
        inputManager_.keyPressed(IK_MoveRight);
    else if(event->key() == Qt::Key::Key_E)
        inputManager_.keyPressed(IK_MoveUp);
    else if(event->key() == Qt::Key::Key_Q)
        inputManager_.keyPressed(IK_MoveDown);
    else if(event->key() == Qt::Key::Key_Shift)
        inputManager_.keyPressed(IK_MoveFast);
}

void MainWindowController::keyReleaseEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key::Key_W)
        inputManager_.keyReleased(IK_MoveForward);
    else if(event->key() == Qt::Key::Key_A)
        inputManager_.keyReleased(IK_MoveLeft);
    else if(event->key() == Qt::Key::Key_S)
        inputManager_.keyReleased(IK_MoveBackwards);
    else if(event->key() == Qt::Key::Key_D)
        inputManager_.keyReleased(IK_MoveRight);
    else if(event->key() == Qt::Key::Key_E)
        inputManager_.keyReleased(IK_MoveUp);
    else if(event->key() == Qt::Key::Key_Q)
        inputManager_.keyReleased(IK_MoveDown);
    else if(event->key() == Qt::Key::Key_Shift)
        inputManager_.keyReleased(IK_MoveFast);
}

void MainWindowController::updateShaderFeature(ShaderFeature feature, int state)
{
    if(state == Qt::CheckState::Checked)
    {
        window_->rendererWidget()->enableFeature(feature);
    }
    else
    {
        window_->rendererWidget()->disableFeature(feature);
    }
}
