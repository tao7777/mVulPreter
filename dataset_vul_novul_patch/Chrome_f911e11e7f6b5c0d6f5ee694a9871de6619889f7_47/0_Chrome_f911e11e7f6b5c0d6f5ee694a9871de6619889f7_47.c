  static void Update(scoped_refptr<ClipPaintPropertyNode> node,
  static void Update(std::unique_ptr<ClipPaintPropertyNode>& node,
                     const ClipPaintPropertyNode& new_parent,
                      const FloatRoundedRect& new_clip_rect) {
    node->Update(new_parent,
                  ClipPaintPropertyNode::State{nullptr, new_clip_rect});
   }
