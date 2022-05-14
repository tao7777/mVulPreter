 float ScrollableShelfView::CalculatePageScrollingOffset(bool forward) const {
  float offset = GetSpaceForIcons() - kArrowButtonGroupWidth -
                  ShelfConfig::Get()->button_size() - GetAppIconEndPadding();
   if (layout_strategy_ == kShowRightArrowButton)
     offset -= (kArrowButtonGroupWidth - GetAppIconEndPadding());
  DCHECK_GT(offset, 0);

  if (!forward)
    offset = -offset;

  return offset;
}
