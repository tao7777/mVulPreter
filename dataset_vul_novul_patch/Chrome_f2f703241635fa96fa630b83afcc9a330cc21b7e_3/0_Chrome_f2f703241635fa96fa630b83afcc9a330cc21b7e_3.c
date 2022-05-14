void ShelfWidget::DelegateView::UpdateOpaqueBackground() {
  const gfx::Rect local_bounds = GetLocalBounds();
  gfx::Rect opaque_background_bounds = local_bounds;

  const Shelf* shelf = shelf_widget_->shelf();
  const ShelfBackgroundType background_type =
      shelf_widget_->GetBackgroundType();

  if (background_type == SHELF_BACKGROUND_APP_LIST) {
    opaque_background_.SetVisible(false);
    UpdateBackgroundBlur();
    return;
  }

  if (!opaque_background_.visible())
    opaque_background_.SetVisible(true);

  const int radius = kShelfRoundedCornerRadius;
  const int safety_margin = 3 * radius;
  opaque_background_bounds.Inset(
      -shelf->SelectValueForShelfAlignment(0, safety_margin, 0), 0,
       -shelf->SelectValueForShelfAlignment(0, 0, safety_margin),
       -shelf->SelectValueForShelfAlignment(safety_margin, 0, 0));
 
  // Show rounded corners except in maximized (which includes split view) mode.
  if (background_type == SHELF_BACKGROUND_MAXIMIZED) {
     mask_ = nullptr;
     opaque_background_.SetMaskLayer(nullptr);
   } else {
    if (!mask_) {
      mask_ = views::Painter::CreatePaintedLayer(
          views::Painter::CreateSolidRoundRectPainter(SK_ColorBLACK, radius));
      mask_->layer()->SetFillsBoundsOpaquely(false);
      opaque_background_.SetMaskLayer(mask_->layer());
    }
    if (mask_->layer()->bounds() != opaque_background_bounds)
      mask_->layer()->SetBounds(opaque_background_bounds);
  }
  opaque_background_.SetBounds(opaque_background_bounds);
  UpdateBackgroundBlur();
  SchedulePaint();
}
