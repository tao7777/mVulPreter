 void FrameView::updateLayoutAndStyleForPainting()
 {
     RefPtr<FrameView> protector(this);
 
     updateLayoutAndStyleIfNeededRecursive();
 
    updateWidgetPositionsIfNeeded();

     if (RenderView* view = renderView()) {
         TRACE_EVENT_INSTANT1(TRACE_DISABLED_BY_DEFAULT("devtools.timeline"), "UpdateLayerTree", "frame", m_frame.get());
        InspectorInstrumentation::willUpdateLayerTree(m_frame.get());

        view->compositor()->updateIfNeededRecursive();

        if (view->compositor()->inCompositingMode() && m_frame->isLocalRoot())
            m_frame->page()->scrollingCoordinator()->updateAfterCompositingChangeIfNeeded();

        updateCompositedSelectionBoundsIfNeeded();

        InspectorInstrumentation::didUpdateLayerTree(m_frame.get());

        invalidateTreeIfNeededRecursive();
    }

    scrollContentsIfNeededRecursive();
    ASSERT(lifecycle().state() == DocumentLifecycle::PaintInvalidationClean);
}
