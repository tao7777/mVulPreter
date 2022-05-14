 void ChunkToLayerMapper::SwitchToChunk(const PaintChunk& chunk) {
   outset_for_raster_effects_ = chunk.outset_for_raster_effects;
 
  const auto& new_chunk_state = chunk.properties;
   if (new_chunk_state == chunk_state_)
     return;
 
  if (new_chunk_state == layer_state_) {
    has_filter_that_moves_pixels_ = false;
    transform_ = TransformationMatrix().Translate(-layer_offset_.x(),
                                                  -layer_offset_.y());
    clip_rect_ = FloatClipRect();
    chunk_state_ = new_chunk_state;
    return;
  }

  if (new_chunk_state.Transform() != chunk_state_.Transform()) {
    transform_ = GeometryMapper::SourceToDestinationProjection(
        new_chunk_state.Transform(), layer_state_.Transform());
    transform_.PostTranslate(-layer_offset_.x(), -layer_offset_.y());
  }

  bool new_has_filter_that_moves_pixels = has_filter_that_moves_pixels_;
  if (new_chunk_state.Effect() != chunk_state_.Effect()) {
    new_has_filter_that_moves_pixels = false;
    for (const auto* effect = new_chunk_state.Effect();
         effect && effect != layer_state_.Effect(); effect = effect->Parent()) {
      if (effect->HasFilterThatMovesPixels()) {
        new_has_filter_that_moves_pixels = true;
        break;
      }
    }
  }

  bool needs_clip_recalculation =
      new_has_filter_that_moves_pixels != has_filter_that_moves_pixels_ ||
      new_chunk_state.Clip() != chunk_state_.Clip();
  if (needs_clip_recalculation) {
    clip_rect_ =
        GeometryMapper::LocalToAncestorClipRect(new_chunk_state, layer_state_);
    if (!clip_rect_.IsInfinite())
      clip_rect_.MoveBy(FloatPoint(-layer_offset_.x(), -layer_offset_.y()));
  }

  chunk_state_ = new_chunk_state;
  has_filter_that_moves_pixels_ = new_has_filter_that_moves_pixels;
}
