 bool InspectorOverlay::isEmpty()
 {
    bool hasAlwaysVisibleElements = m_highlightNode || m_eventTargetNode || m_highlightQuad || !m_size.isEmpty() || m_drawViewSize;
     bool hasInvisibleInInspectModeElements = !m_pausedInDebuggerMessage.isNull();
     return !(hasAlwaysVisibleElements || (hasInvisibleInInspectModeElements && !m_inspectModeEnabled));
 }
