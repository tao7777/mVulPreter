 bool PaintArtifactCompositor::MightOverlap(const PendingLayer& layer_a,
                                            const PendingLayer& layer_b) {
   FloatClipRect bounds_a(layer_a.bounds);
  GeometryMapper::LocalToAncestorVisualRect(
      layer_a.property_tree_state, PropertyTreeState::Root(), bounds_a);
   FloatClipRect bounds_b(layer_b.bounds);
  GeometryMapper::LocalToAncestorVisualRect(
      layer_b.property_tree_state, PropertyTreeState::Root(), bounds_b);
 
   return bounds_a.Rect().Intersects(bounds_b.Rect());
 }
