#pragma once

#include <string>
#include <QRadioButton>

using namespace std;

#include "Shader.hpp"

class OverlayToggle : public QRadioButton
{
public:
    OverlayToggle(int index, const char* name);
    
    // Feature to toggle
    int index() const { return index_; }
    
    // Feature name
    string name() const { return name_; }
    
    // Check box status
    bool overlayEnabled() const;
    
private:
    int index_;
    string name_;
};
