bool LayoutSVGViewportContainer::calculateLocalTransform()
void LayoutSVGViewportContainer::setNeedsTransformUpdate()
{
    setMayNeedPaintInvalidationSubtree();
    m_needsTransformUpdate = true;
}

static std::pair<double, double> scaleReference(const AffineTransform& transform)
{
    return std::make_pair(transform.xScaleSquared(), transform.yScaleSquared());
}

LayoutSVGContainer::TransformChange LayoutSVGViewportContainer::calculateLocalTransform()
 {
     if (!m_needsTransformUpdate)
        return TransformChange::None;
 
    std::pair<double, double> oldScale = scaleReference(m_localToParentTransform);
     m_localToParentTransform = AffineTransform::translation(m_viewport.x(), m_viewport.y()) * viewportTransform();
     m_needsTransformUpdate = false;
    return scaleReference(m_localToParentTransform) != oldScale
        ? TransformChange::Full : TransformChange::ScaleInvariant;
 }
