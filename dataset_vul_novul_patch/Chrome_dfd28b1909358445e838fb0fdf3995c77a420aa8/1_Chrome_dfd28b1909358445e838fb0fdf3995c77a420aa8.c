gfx::Insets ScrollableShelfView::CalculateEdgePadding() const {
  if (ShouldApplyDisplayCentering())
    return CalculatePaddingForDisplayCentering();
 
   const int icons_size = shelf_view_->GetSizeOfAppIcons(
       shelf_view_->number_of_visible_apps(), false);
  gfx::Insets padding_insets(
      /*vertical= */ 0,
      /*horizontal= */ ShelfConfig::Get()->app_icon_group_margin());
 
   const int available_size_for_app_icons =
       (GetShelf()->IsHorizontalAlignment() ? width() : height()) -
      2 * ShelfConfig::Get()->app_icon_group_margin();

  int gap =
      layout_strategy_ == kNotShowArrowButtons
           ? available_size_for_app_icons - icons_size  // shelf centering
           : CalculateOverflowPadding(available_size_for_app_icons);  // overflow
 
  padding_insets.set_left(padding_insets.left() + gap / 2);
  padding_insets.set_right(padding_insets.right() +
                           (gap % 2 ? gap / 2 + 1 : gap / 2));
 
   return padding_insets;
 }
