 bool PaintArtifactCompositor::MightOverlap(const PendingLayer& layer_a,
                                            const PendingLayer& layer_b) {
  PropertyTreeState root_property_tree_state(TransformPaintPropertyNode::Root(),
                                             ClipPaintPropertyNode::Root(),
                                             EffectPaintPropertyNode::Root());
   FloatClipRect bounds_a(layer_a.bounds);
  GeometryMapper::LocalToAncestorVisualRect(layer_a.property_tree_state,
                                            root_property_tree_state, bounds_a);
   FloatClipRect bounds_b(layer_b.bounds);
  GeometryMapper::LocalToAncestorVisualRect(layer_b.property_tree_state,
                                            root_property_tree_state, bounds_b);
 
   return bounds_a.Rect().Intersects(bounds_b.Rect());
 }
