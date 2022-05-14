gfx::Vector2d LayerTreeHost::DistributeScrollOffsetToViewports(
    const gfx::Vector2d offset,
    Layer* layer) {
  DCHECK(layer);
  if (layer != outer_viewport_scroll_layer_.get())
    return offset;
  gfx::Vector2d inner_viewport_offset =
      inner_viewport_scroll_layer_->scroll_offset();
  gfx::Vector2d outer_viewport_offset =
      outer_viewport_scroll_layer_->scroll_offset();
  if (offset == inner_viewport_offset + outer_viewport_offset) {
    return outer_viewport_offset;
  }
  gfx::Vector2d max_outer_viewport_scroll_offset =
      outer_viewport_scroll_layer_->MaxScrollOffset();
  gfx::Vector2d max_inner_viewport_scroll_offset =
      inner_viewport_scroll_layer_->MaxScrollOffset();
  outer_viewport_offset = offset - inner_viewport_offset;
  outer_viewport_offset.SetToMin(max_outer_viewport_scroll_offset);
  outer_viewport_offset.SetToMax(gfx::Vector2d());
  inner_viewport_offset = offset - outer_viewport_offset;
  inner_viewport_offset.SetToMin(max_inner_viewport_scroll_offset);
  inner_viewport_offset.SetToMax(gfx::Vector2d());
  inner_viewport_scroll_layer_->SetScrollOffset(inner_viewport_offset);
  return outer_viewport_offset;
}
