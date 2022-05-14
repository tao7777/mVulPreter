TransformPaintPropertyNode* TransformPaintPropertyNode::Root() {
const TransformPaintPropertyNode& TransformPaintPropertyNode::Root() {
  DEFINE_STATIC_LOCAL(
       TransformPaintPropertyNode, root,
      (nullptr,
       State{TransformationMatrix(), FloatPoint3D(), false,
             BackfaceVisibility::kVisible, 0, CompositingReason::kNone,
             CompositorElementId(), &ScrollPaintPropertyNode::Root()}));
   return root;
 }
