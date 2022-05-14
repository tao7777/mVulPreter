ClipPaintPropertyNode* ClipPaintPropertyNode::Root() {
  DEFINE_STATIC_REF(
       ClipPaintPropertyNode, root,
      (ClipPaintPropertyNode::Create(
          nullptr, State{TransformPaintPropertyNode::Root(),
                         FloatRoundedRect(LayoutRect::InfiniteIntRect())})));
   return root;
 }
