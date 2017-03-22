#pragma once

#include <string>
#include <QCheckBox>

using namespace std;

#include "Shader.hpp"

class FeatureToggle : public QCheckBox
{
public:
    FeatureToggle(ShaderFeature feature, const char* name);
    
    // Feature to toggle
    ShaderFeature feature() const { return feature_; }
    
    // Feature name
    string name() const { return name_; }
    
    // Check box status
    bool featureEnabled() const;
    
private:
    ShaderFeature feature_;
    string name_;
};
