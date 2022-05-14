void LauncherView::SetAlignment(ShelfAlignment alignment) {
  if (alignment_ == alignment)
    return;
  alignment_ = alignment;
   UpdateFirstButtonPadding();
   LayoutToIdealBounds();
   tooltip_->SetArrowLocation(alignment_);
 }
