 void ScrollHitTestDisplayItem::Record(
     GraphicsContext& context,
     const DisplayItemClient& client,
     DisplayItem::Type type,
    const TransformPaintPropertyNode& scroll_offset_node) {
   PaintController& paint_controller = context.GetPaintController();
 
   DCHECK_NE(paint_controller.CurrentPaintChunkProperties().Transform(),
            &scroll_offset_node);
 
   if (paint_controller.DisplayItemConstructionIsDisabled())
     return;
 
   paint_controller.CreateAndAppend<ScrollHitTestDisplayItem>(
      client, type, scroll_offset_node);
 }
