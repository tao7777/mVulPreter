void LauncherView::SetAlignment(ShelfAlignment alignment) {
  if (alignment_ == alignment)
    return;
  alignment_ = alignment;
   UpdateFirstButtonPadding();
   LayoutToIdealBounds();
   tooltip_->SetArrowLocation(alignment_);
  if (overflow_bubble_.get())
    overflow_bubble_->Hide();
 }
