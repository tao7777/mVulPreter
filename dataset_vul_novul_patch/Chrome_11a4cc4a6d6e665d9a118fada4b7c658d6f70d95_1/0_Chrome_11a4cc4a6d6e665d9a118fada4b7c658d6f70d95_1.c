void RenderLayerScrollableArea::setScrollOffset(const IntPoint& newScrollOffset)
{
    if (!box().isMarquee()) {
        if (m_scrollDimensionsDirty)
            computeScrollDimensions();
    }

    if (scrollOffset() == toIntSize(newScrollOffset))
        return;

    setScrollOffset(toIntSize(newScrollOffset));

    LocalFrame* frame = box().frame();
    ASSERT(frame);

    RefPtr<FrameView> frameView = box().frameView();

    TRACE_EVENT1(TRACE_DISABLED_BY_DEFAULT("devtools.timeline"), "ScrollLayer", "data", InspectorScrollLayerEvent::data(&box()));
    InspectorInstrumentation::willScrollLayer(&box());

    const RenderLayerModelObject* paintInvalidationContainer = box().containerForPaintInvalidation();

    if (!frameView->isInPerformLayout()) {
        layer()->clipper().clearClipRectsIncludingDescendants();
         box().setPreviousPaintInvalidationRect(box().boundsRectForPaintInvalidation(paintInvalidationContainer));
         frameView->updateAnnotatedRegions();
        frameView->setNeedsUpdateWidgetPositions();
         updateCompositingLayersAfterScroll();
     }
 
    frame->selection().setCaretRectNeedsUpdate();

    FloatQuad quadForFakeMouseMoveEvent = FloatQuad(layer()->renderer()->previousPaintInvalidationRect());

    quadForFakeMouseMoveEvent = paintInvalidationContainer->localToAbsoluteQuad(quadForFakeMouseMoveEvent);
    frame->eventHandler().dispatchFakeMouseMoveEventSoonInQuad(quadForFakeMouseMoveEvent);

    bool requiresPaintInvalidation = true;

    if (!box().isMarquee() && box().view()->compositor()->inCompositingMode()) {
        DisableCompositingQueryAsserts disabler;
        bool onlyScrolledCompositedLayers = scrollsOverflow()
            && !layer()->hasVisibleNonLayerContent()
            && !layer()->hasNonCompositedChild()
            && !layer()->hasBlockSelectionGapBounds()
            && box().style()->backgroundLayers().attachment() != LocalBackgroundAttachment;

        if (usesCompositedScrolling() || onlyScrolledCompositedLayers)
            requiresPaintInvalidation = false;
    }

    if (requiresPaintInvalidation) {
        if (box().frameView()->isInPerformLayout())
            box().setShouldDoFullPaintInvalidation(true);
        else
            box().invalidatePaintUsingContainer(paintInvalidationContainer, layer()->renderer()->previousPaintInvalidationRect(), InvalidationScroll);
    }

    if (box().node())
        box().node()->document().enqueueScrollEventForNode(box().node());

    if (AXObjectCache* cache = box().document().existingAXObjectCache())
        cache->handleScrollPositionChanged(&box());

    InspectorInstrumentation::didScrollLayer(&box());
}
