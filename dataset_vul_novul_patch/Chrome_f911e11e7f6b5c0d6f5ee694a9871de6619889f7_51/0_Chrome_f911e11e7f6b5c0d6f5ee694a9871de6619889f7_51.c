 ScrollHitTestDisplayItem::ScrollHitTestDisplayItem(
     const DisplayItemClient& client,
     Type type,
    const TransformPaintPropertyNode& scroll_offset_node)
     : DisplayItem(client, type, sizeof(*this)),
      scroll_offset_node_(scroll_offset_node) {
   DCHECK(RuntimeEnabledFeatures::SlimmingPaintV2Enabled());
   DCHECK(IsScrollHitTestType(type));
  DCHECK(scroll_offset_node_.ScrollNode());
 }
