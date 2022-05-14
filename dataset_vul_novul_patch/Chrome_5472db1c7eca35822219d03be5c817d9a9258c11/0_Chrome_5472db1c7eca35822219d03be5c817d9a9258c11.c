void PaintLayerScrollableArea::UpdateCompositingLayersAfterScroll() {
  PaintLayerCompositor* compositor = GetLayoutBox()->View()->Compositor();
  if (!compositor->InCompositingMode())
    return;

  if (UsesCompositedScrolling()) {
     DCHECK(Layer()->HasCompositedLayerMapping());
     ScrollingCoordinator* scrolling_coordinator = GetScrollingCoordinator();
     bool handled_scroll =
        (Layer()->IsRootLayer() ||
         RuntimeEnabledFeatures::BlinkGenPropertyTreesEnabled()) &&
        scrolling_coordinator &&
         scrolling_coordinator->UpdateCompositedScrollOffset(this);
 
     if (!handled_scroll) {
      if (!RuntimeEnabledFeatures::BlinkGenPropertyTreesEnabled()) {
        Layer()->GetCompositedLayerMapping()->SetNeedsGraphicsLayerUpdate(
            kGraphicsLayerUpdateSubtree);
      }
      compositor->SetNeedsCompositingUpdate(
          kCompositingUpdateAfterGeometryChange);
    }

    if (Layer()->IsRootLayer()) {
      LocalFrame* frame = GetLayoutBox()->GetFrame();
      if (frame && frame->View() &&
          frame->View()->HasViewportConstrainedObjects()) {
        Layer()->SetNeedsCompositingInputsUpdate();
      }
    }
  } else {
    Layer()->SetNeedsCompositingInputsUpdate();
  }
}
