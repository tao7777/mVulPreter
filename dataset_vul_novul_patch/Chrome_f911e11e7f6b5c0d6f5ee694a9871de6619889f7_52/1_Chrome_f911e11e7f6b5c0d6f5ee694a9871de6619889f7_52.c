ScrollPaintPropertyNode* ScrollPaintPropertyNode::Root() {
  DEFINE_STATIC_REF(ScrollPaintPropertyNode, root,
                    (ScrollPaintPropertyNode::Create(nullptr, State{})));
   return root;
 }
