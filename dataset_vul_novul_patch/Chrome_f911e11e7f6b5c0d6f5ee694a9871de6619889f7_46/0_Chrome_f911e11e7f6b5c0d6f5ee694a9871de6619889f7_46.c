void PaintController::CopyCachedSubsequence(size_t begin_index,
                                            size_t end_index) {
  DCHECK(!RuntimeEnabledFeatures::PaintUnderInvalidationCheckingEnabled());

  base::AutoReset<size_t> subsequence_begin_index(
      &current_cached_subsequence_begin_index_in_new_list_,
      new_display_item_list_.size());
  DisplayItem* cached_item =
      &current_paint_artifact_.GetDisplayItemList()[begin_index];

  Vector<PaintChunk>::const_iterator cached_chunk;
  base::Optional<PropertyTreeState> properties_before_subsequence;
  if (RuntimeEnabledFeatures::SlimmingPaintV175Enabled()) {
    cached_chunk =
        current_paint_artifact_.FindChunkByDisplayItemIndex(begin_index);
    DCHECK(cached_chunk != current_paint_artifact_.PaintChunks().end());

     properties_before_subsequence =
         new_paint_chunks_.CurrentPaintChunkProperties();
     UpdateCurrentPaintChunkPropertiesUsingIdWithFragment(
        cached_chunk->id, cached_chunk->properties);
   } else {
     cached_chunk = current_paint_artifact_.PaintChunks().begin();
  }

  for (size_t current_index = begin_index; current_index < end_index;
       ++current_index) {
    cached_item = &current_paint_artifact_.GetDisplayItemList()[current_index];
    SECURITY_CHECK(!cached_item->IsTombstone());
#if DCHECK_IS_ON()
    DCHECK(cached_item->Client().IsAlive());
#endif

    if (RuntimeEnabledFeatures::SlimmingPaintV175Enabled() &&
        current_index == cached_chunk->end_index) {
      ++cached_chunk;
       DCHECK(cached_chunk != current_paint_artifact_.PaintChunks().end());
       new_paint_chunks_.ForceNewChunk();
       UpdateCurrentPaintChunkPropertiesUsingIdWithFragment(
          cached_chunk->id, cached_chunk->properties);
     }
 
 #if DCHECK_IS_ON()
    if (cached_item->VisualRect() !=
        FloatRect(cached_item->Client().VisualRect())) {
      LOG(ERROR) << "Visual rect changed in a cached subsequence: "
                 << cached_item->Client().DebugName()
                 << " old=" << cached_item->VisualRect().ToString()
                 << " new=" << cached_item->Client().VisualRect().ToString();
    }
#endif

    ProcessNewItem(MoveItemFromCurrentListToNewList(current_index));
    if (RuntimeEnabledFeatures::SlimmingPaintV175Enabled()) {
      DCHECK((!new_paint_chunks_.LastChunk().is_cacheable &&
              !cached_chunk->is_cacheable) ||
             new_paint_chunks_.LastChunk().Matches(*cached_chunk));
    }
  }

  if (RuntimeEnabledFeatures::PaintUnderInvalidationCheckingEnabled()) {
    under_invalidation_checking_end_ = end_index;
    DCHECK(IsCheckingUnderInvalidation());
  } else if (RuntimeEnabledFeatures::SlimmingPaintV175Enabled()) {
    new_paint_chunks_.ForceNewChunk();
    UpdateCurrentPaintChunkProperties(base::nullopt,
                                      *properties_before_subsequence);
  }
}
