void PropertyTreeManager::SetupRootTransformNode() {
  cc::TransformTree& transform_tree = property_trees_.transform_tree;
  transform_tree.clear();
  property_trees_.element_id_to_transform_node_index.clear();
  cc::TransformNode& transform_node = *transform_tree.Node(
      transform_tree.Insert(cc::TransformNode(), kRealRootNodeId));
  DCHECK_EQ(transform_node.id, kSecondaryRootNodeId);
  transform_node.source_node_id = transform_node.parent_id;

  float device_scale_factor =
      root_layer_->layer_tree_host()->device_scale_factor();
  gfx::Transform to_screen;
  to_screen.Scale(device_scale_factor, device_scale_factor);
  transform_tree.SetToScreen(kRealRootNodeId, to_screen);
  gfx::Transform from_screen;
  bool invertible = to_screen.GetInverse(&from_screen);
  DCHECK(invertible);
   transform_tree.SetFromScreen(kRealRootNodeId, from_screen);
   transform_tree.set_needs_update(true);
 
  transform_node_map_.Set(&TransformPaintPropertyNode::Root(),
                           transform_node.id);
   root_layer_->SetTransformTreeIndex(transform_node.id);
 }
