 const PropertyTreeState& PropertyTreeState::Root() {
   DEFINE_STATIC_LOCAL(
      PropertyTreeState, root,
      (&TransformPaintPropertyNode::Root(), &ClipPaintPropertyNode::Root(),
       &EffectPaintPropertyNode::Root()));
  return root;
 }
