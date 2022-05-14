void InspectorOverlay::hide()
{
    m_timer.stop();
    m_highlightNode.clear();
    m_eventTargetNode.clear();
    m_highlightQuad.clear();
    m_pausedInDebuggerMessage = String();
     m_size = IntSize();
     m_drawViewSize = false;
     m_drawViewSizeWithGrid = false;
    m_overrides = 0;
    m_overridesTopOffset = 0;
     update();
 }
