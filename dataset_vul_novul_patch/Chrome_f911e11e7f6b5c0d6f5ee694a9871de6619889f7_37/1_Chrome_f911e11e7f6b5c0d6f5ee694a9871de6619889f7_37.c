void PropertyTreeManager::SetupRootClipNode() {
  cc::ClipTree& clip_tree = property_trees_.clip_tree;
  clip_tree.clear();
  cc::ClipNode& clip_node =
      *clip_tree.Node(clip_tree.Insert(cc::ClipNode(), kRealRootNodeId));
  DCHECK_EQ(clip_node.id, kSecondaryRootNodeId);

  clip_node.clip_type = cc::ClipNode::ClipType::APPLIES_LOCAL_CLIP;
  clip_node.clip = gfx::RectF(
       gfx::SizeF(root_layer_->layer_tree_host()->device_viewport_size()));
   clip_node.transform_id = kRealRootNodeId;
 
  clip_node_map_.Set(ClipPaintPropertyNode::Root(), clip_node.id);
   root_layer_->SetClipTreeIndex(clip_node.id);
 }
