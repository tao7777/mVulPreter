 CompositedLayerRasterInvalidator::ChunkPropertiesChanged(
    const PropertyTreeState& new_chunk_state,
     const PaintChunkInfo& new_chunk,
     const PaintChunkInfo& old_chunk,
     const PropertyTreeState& layer_state) const {
  if (!ApproximatelyEqual(new_chunk.chunk_to_layer_transform,
                          old_chunk.chunk_to_layer_transform))
    return PaintInvalidationReason::kPaintProperty;

  if (new_chunk_state.Effect() != old_chunk.effect_state ||
      new_chunk_state.Effect()->Changed(*layer_state.Effect()))
    return PaintInvalidationReason::kPaintProperty;

  if (new_chunk.chunk_to_layer_clip.IsTight() &&
      old_chunk.chunk_to_layer_clip.IsTight()) {
    if (new_chunk.chunk_to_layer_clip == old_chunk.chunk_to_layer_clip)
      return PaintInvalidationReason::kNone;
    if (ClipByLayerBounds(new_chunk.chunk_to_layer_clip.Rect()) ==
        ClipByLayerBounds(old_chunk.chunk_to_layer_clip.Rect()))
      return PaintInvalidationReason::kNone;
    return PaintInvalidationReason::kIncremental;
  }

  if (new_chunk_state.Clip() != old_chunk.clip_state ||
      new_chunk_state.Clip()->Changed(*layer_state.Clip()))
    return PaintInvalidationReason::kPaintProperty;

  return PaintInvalidationReason::kNone;
}
