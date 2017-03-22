#include "FeatureToggle.hpp"

FeatureToggle::FeatureToggle(ShaderFeature feature, const char* name)
    : feature_(feature),
    name_(name)
{
    // Setup the label
    setText(name);
    
    // All features are enabled by default
    setChecked(true);
}

bool FeatureToggle::featureEnabled() const
{
    return QCheckBox::checkState() == Qt::CheckState::Checked;
}
