 ScrollHitTestDisplayItem::ScrollHitTestDisplayItem(
     const DisplayItemClient& client,
     Type type,
    scoped_refptr<const TransformPaintPropertyNode> scroll_offset_node)
     : DisplayItem(client, type, sizeof(*this)),
      scroll_offset_node_(std::move(scroll_offset_node)) {
   DCHECK(RuntimeEnabledFeatures::SlimmingPaintV2Enabled());
   DCHECK(IsScrollHitTestType(type));
  DCHECK(scroll_offset_node_->ScrollNode());
 }
