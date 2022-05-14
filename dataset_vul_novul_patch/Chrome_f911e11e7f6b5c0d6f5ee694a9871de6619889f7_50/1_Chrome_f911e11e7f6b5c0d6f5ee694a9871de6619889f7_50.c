 void ScrollHitTestDisplayItem::Record(
     GraphicsContext& context,
     const DisplayItemClient& client,
     DisplayItem::Type type,
    scoped_refptr<const TransformPaintPropertyNode> scroll_offset_node) {
   PaintController& paint_controller = context.GetPaintController();
 
   DCHECK_NE(paint_controller.CurrentPaintChunkProperties().Transform(),
            scroll_offset_node.get());
 
   if (paint_controller.DisplayItemConstructionIsDisabled())
     return;
 
   paint_controller.CreateAndAppend<ScrollHitTestDisplayItem>(
      client, type, std::move(scroll_offset_node));
 }
