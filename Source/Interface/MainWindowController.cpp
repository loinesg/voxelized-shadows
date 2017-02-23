#include "MainWindowController.hpp"

MainWindowController::MainWindowController(MainWindow* window)
    : window_(window),
    inputManager_(),
    mouseDragging_(false),
    mousePosition_(Vector2(0, 0))
{
    window_->rendererWidget()->setFocus();
    window_->rendererWidget()->installEventFilter(this);
}

MainWindowController::~MainWindowController()
{
    
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
