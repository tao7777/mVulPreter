void PropertyTreeManager::SetupRootScrollNode() {
  cc::ScrollTree& scroll_tree = property_trees_.scroll_tree;
  scroll_tree.clear();
  property_trees_.element_id_to_scroll_node_index.clear();
  cc::ScrollNode& scroll_node =
      *scroll_tree.Node(scroll_tree.Insert(cc::ScrollNode(), kRealRootNodeId));
   DCHECK_EQ(scroll_node.id, kSecondaryRootNodeId);
   scroll_node.transform_id = kSecondaryRootNodeId;
 
  scroll_node_map_.Set(&ScrollPaintPropertyNode::Root(), scroll_node.id);
   root_layer_->SetScrollTreeIndex(scroll_node.id);
 }
