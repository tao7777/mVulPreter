 TestPaintArtifact& TestPaintArtifact::ScrollHitTest(
     DisplayItemClient& client,
    const TransformPaintPropertyNode& scroll_offset) {
   display_item_list_.AllocateAndConstruct<ScrollHitTestDisplayItem>(
      client, DisplayItem::kScrollHitTest, scroll_offset);
   return *this;
 }
