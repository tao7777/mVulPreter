void InspectorOverlay::setOverride(OverrideType type, bool enabled)
{
    bool currentEnabled = m_overrides & type;
    if (currentEnabled == enabled)
        return;
    if (enabled)
        m_overrides |= type;
    else
        m_overrides &= ~type;
    update();
}
