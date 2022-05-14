void ConversionContext::Convert(const PaintChunkSubset& paint_chunks,
                                const DisplayItemList& display_items) {
  for (const auto& chunk : paint_chunks) {
    const auto& chunk_state = chunk.properties;
    bool switched_to_chunk_state = false;

    for (const auto& item : display_items.ItemsInPaintChunk(chunk)) {
      DCHECK(item.IsDrawing());
      auto record =
          static_cast<const DrawingDisplayItem&>(item).GetPaintRecord();
       if ((!record || record->size() == 0) &&
          chunk_state.Effect() == &EffectPaintPropertyNode::Root()) {
         continue;
       }
 
      TranslateForLayerOffsetOnce();
      if (!switched_to_chunk_state) {
        SwitchToChunkState(chunk);
        switched_to_chunk_state = true;
      }

      cc_list_.StartPaint();
      if (record && record->size() != 0)
        cc_list_.push<cc::DrawRecordOp>(std::move(record));
      cc_list_.EndPaintOfUnpaired(
          chunk_to_layer_mapper_.MapVisualRect(item.VisualRect()));
    }
    UpdateEffectBounds(chunk.bounds, chunk_state.Transform());
  }
}
