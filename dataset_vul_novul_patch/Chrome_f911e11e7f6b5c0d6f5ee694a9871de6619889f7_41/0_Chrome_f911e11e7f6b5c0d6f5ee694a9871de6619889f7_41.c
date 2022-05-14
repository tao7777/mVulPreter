ClipPaintPropertyNode* ClipPaintPropertyNode::Root() {
const ClipPaintPropertyNode& ClipPaintPropertyNode::Root() {
  DEFINE_STATIC_LOCAL(
       ClipPaintPropertyNode, root,
      (nullptr, State{&TransformPaintPropertyNode::Root(),
                      FloatRoundedRect(LayoutRect::InfiniteIntRect())}));
   return root;
 }
