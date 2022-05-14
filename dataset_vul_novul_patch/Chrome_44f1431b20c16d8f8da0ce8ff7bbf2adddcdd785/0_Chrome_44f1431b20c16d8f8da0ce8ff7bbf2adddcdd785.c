void LayoutSVGContainer::layout()
{
    ASSERT(needsLayout());
    LayoutAnalyzer::Scope analyzer(*this);

     calcViewport();
 
    TransformChange transformChange = calculateLocalTransform();
    m_didScreenScaleFactorChange =
        transformChange == TransformChange::Full || SVGLayoutSupport::screenScaleFactorChanged(parent());
 
     determineIfLayoutSizeChanged();

    bool layoutSizeChanged = element()->hasRelativeLengths()
        && SVGLayoutSupport::layoutSizeOfNearestViewportChanged(this);

    SVGLayoutSupport::layoutChildren(firstChild(), false, m_didScreenScaleFactorChange, layoutSizeChanged);

     if (everHadLayout() && needsLayout())
         SVGResourcesCache::clientLayoutChanged(this);
 
    if (m_needsBoundariesUpdate || transformChange != TransformChange::None) {
         updateCachedBoundaries();
         m_needsBoundariesUpdate = false;
 
        LayoutSVGModelObject::setNeedsBoundariesUpdate();
    }

    ASSERT(!m_needsBoundariesUpdate);
    clearNeedsLayout();
}
