#include "OverlayToggle.hpp"

OverlayToggle::OverlayToggle(int index, const char* name)
    : index_(index),
    name_(name)
{
    // Setup the label
    setText(name);
    
    // Make feature -1 checked by default.
    // -1 is the "No Overlay" option
    setChecked(index == -1);
}

bool OverlayToggle::overlayEnabled() const
{
    return QRadioButton::isChecked();
}
