int ScrollableShelfView::CalculateScrollUpperBound() const {
  if (layout_strategy_ == kNotShowArrowButtons)
     return 0;
 
  int available_length = space_for_icons_ - 2 * GetAppIconEndPadding();
 
   const gfx::Size shelf_preferred_size(
      shelf_container_view_->GetPreferredSize());
  const int preferred_length =
      (GetShelf()->IsHorizontalAlignment() ? shelf_preferred_size.width()
                                           : shelf_preferred_size.height());

  return std::max(0, preferred_length - available_length);
}
