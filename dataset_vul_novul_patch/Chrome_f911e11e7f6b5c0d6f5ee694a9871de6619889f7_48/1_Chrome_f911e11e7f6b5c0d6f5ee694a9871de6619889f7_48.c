 const PropertyTreeState& PropertyTreeState::Root() {
   DEFINE_STATIC_LOCAL(
      std::unique_ptr<PropertyTreeState>, root,
      (std::make_unique<PropertyTreeState>(TransformPaintPropertyNode::Root(),
                                           ClipPaintPropertyNode::Root(),
                                           EffectPaintPropertyNode::Root())));
  return *root;
 }
