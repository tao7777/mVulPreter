void LayoutSVGContainer::layout()
{
    ASSERT(needsLayout());
    LayoutAnalyzer::Scope analyzer(*this);

     calcViewport();
 
    bool updatedTransform = calculateLocalTransform();
    m_didScreenScaleFactorChange = updatedTransform || SVGLayoutSupport::screenScaleFactorChanged(parent());
 
     determineIfLayoutSizeChanged();

    bool layoutSizeChanged = element()->hasRelativeLengths()
        && SVGLayoutSupport::layoutSizeOfNearestViewportChanged(this);

    SVGLayoutSupport::layoutChildren(firstChild(), false, m_didScreenScaleFactorChange, layoutSizeChanged);

     if (everHadLayout() && needsLayout())
         SVGResourcesCache::clientLayoutChanged(this);
 
    if (m_needsBoundariesUpdate || updatedTransform) {
         updateCachedBoundaries();
         m_needsBoundariesUpdate = false;
 
        LayoutSVGModelObject::setNeedsBoundariesUpdate();
    }

    ASSERT(!m_needsBoundariesUpdate);
    clearNeedsLayout();
}
