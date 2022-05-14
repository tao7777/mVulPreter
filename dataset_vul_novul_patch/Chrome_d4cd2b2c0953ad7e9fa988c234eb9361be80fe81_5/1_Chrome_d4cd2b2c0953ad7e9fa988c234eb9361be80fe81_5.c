void InspectorOverlay::setOverridesTopOffset(int offset)
{
    m_overridesTopOffset = offset;
    if (m_overrides)
        update();
}
