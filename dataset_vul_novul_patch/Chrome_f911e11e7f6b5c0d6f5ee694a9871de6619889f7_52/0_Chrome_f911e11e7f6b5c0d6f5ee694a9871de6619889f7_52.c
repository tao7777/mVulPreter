ScrollPaintPropertyNode* ScrollPaintPropertyNode::Root() {
const ScrollPaintPropertyNode& ScrollPaintPropertyNode::Root() {
  DEFINE_STATIC_LOCAL(ScrollPaintPropertyNode, root, (nullptr, State{}));
   return root;
 }
