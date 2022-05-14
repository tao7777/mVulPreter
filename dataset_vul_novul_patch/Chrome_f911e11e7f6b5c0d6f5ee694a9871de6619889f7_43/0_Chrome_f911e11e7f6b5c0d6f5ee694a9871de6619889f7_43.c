EffectPaintPropertyNode* EffectPaintPropertyNode::Root() {
const EffectPaintPropertyNode& EffectPaintPropertyNode::Root() {
  DEFINE_STATIC_LOCAL(EffectPaintPropertyNode, root,
                      (nullptr, State{&TransformPaintPropertyNode::Root(),
                                      &ClipPaintPropertyNode::Root()}));
   return root;
 }
