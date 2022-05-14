gfx::Insets ScrollableShelfView::CalculateEdgePadding() const {
  if (ShouldApplyDisplayCentering())
    return CalculatePaddingForDisplayCentering();
 
   const int icons_size = shelf_view_->GetSizeOfAppIcons(
       shelf_view_->number_of_visible_apps(), false);
  const int base_padding = ShelfConfig::Get()->app_icon_group_margin();
 
   const int available_size_for_app_icons =
       (GetShelf()->IsHorizontalAlignment() ? width() : height()) -
      2 * ShelfConfig::Get()->app_icon_group_margin();

  int gap =
      layout_strategy_ == kNotShowArrowButtons
           ? available_size_for_app_icons - icons_size  // shelf centering
           : CalculateOverflowPadding(available_size_for_app_icons);  // overflow
 
  // Calculates the paddings before/after the visible area of scrollable shelf.
  const int before_padding = base_padding + gap / 2;
  const int after_padding = base_padding + (gap % 2 ? gap / 2 + 1 : gap / 2);

  gfx::Insets padding_insets;
  if (GetShelf()->IsHorizontalAlignment()) {
    padding_insets =
        gfx::Insets(/*top=*/0, before_padding, /*bottom=*/0, after_padding);
  } else {
    padding_insets =
        gfx::Insets(before_padding, /*left=*/0, after_padding, /*right=*/0);
  }
 
   return padding_insets;
 }
