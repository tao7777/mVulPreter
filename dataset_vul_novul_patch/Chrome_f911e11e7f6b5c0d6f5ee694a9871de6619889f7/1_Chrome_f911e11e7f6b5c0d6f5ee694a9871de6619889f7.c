void BlockPainter::PaintScrollHitTestDisplayItem(const PaintInfo& paint_info) {
  DCHECK(RuntimeEnabledFeatures::SlimmingPaintV2Enabled());

  if (paint_info.GetGlobalPaintFlags() & kGlobalPaintFlattenCompositingLayers)
    return;


  const auto* fragment = paint_info.FragmentToPaint(layout_block_);
  const auto* properties = fragment ? fragment->PaintProperties() : nullptr;

  if (properties && properties->Scroll()) {
    DCHECK(properties->ScrollTranslation());
    ScopedPaintChunkProperties scroll_hit_test_properties(
        paint_info.context.GetPaintController(),
        fragment->LocalBorderBoxProperties(), layout_block_,
         DisplayItem::kScrollHitTest);
     ScrollHitTestDisplayItem::Record(paint_info.context, layout_block_,
                                      DisplayItem::kScrollHitTest,
                                     properties->ScrollTranslation());
   }
 }
