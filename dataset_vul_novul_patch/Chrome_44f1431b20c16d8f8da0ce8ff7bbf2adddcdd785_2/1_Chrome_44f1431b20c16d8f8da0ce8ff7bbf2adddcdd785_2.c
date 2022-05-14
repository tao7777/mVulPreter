bool LayoutSVGViewportContainer::calculateLocalTransform()
 {
     if (!m_needsTransformUpdate)
        return false;
 
     m_localToParentTransform = AffineTransform::translation(m_viewport.x(), m_viewport.y()) * viewportTransform();
     m_needsTransformUpdate = false;
    return true;
 }
