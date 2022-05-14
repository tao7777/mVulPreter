TransformPaintPropertyNode* TransformPaintPropertyNode::Root() {
  DEFINE_STATIC_REF(
       TransformPaintPropertyNode, root,
      base::AdoptRef(new TransformPaintPropertyNode(
          nullptr,
          State{TransformationMatrix(), FloatPoint3D(), false,
                BackfaceVisibility::kVisible, 0, CompositingReason::kNone,
                CompositorElementId(), ScrollPaintPropertyNode::Root()})));
   return root;
 }
