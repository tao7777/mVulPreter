 void GM2TabStyle::PaintTabBackgroundFill(gfx::Canvas* canvas,
                                         bool active,
                                          bool paint_hover_effect,
                                         SkColor active_color,
                                         SkColor inactive_color,
                                          int fill_id,
                                          int y_inset) const {
  const SkPath fill_path =
      GetPath(PathType::kFill, canvas->image_scale(), active);
   gfx::ScopedCanvas scoped_canvas(canvas);
   const float scale = canvas->UndoDeviceScaleFactor();
 
   canvas->ClipPath(fill_path, true);
 
  if (active || !fill_id) {
     cc::PaintFlags flags;
     flags.setAntiAlias(true);
    flags.setColor(active ? active_color : inactive_color);
     canvas->DrawRect(gfx::ScaleToEnclosingRect(tab_->GetLocalBounds(), scale),
                      flags);
   }

  if (fill_id) {
    gfx::ScopedCanvas scale_scoper(canvas);
    canvas->sk_canvas()->scale(scale, scale);
    canvas->TileImageInt(*tab_->GetThemeProvider()->GetImageSkiaNamed(fill_id),
                         tab_->GetMirroredX() + tab_->background_offset(), 0, 0,
                         y_inset, tab_->width(), tab_->height());
  }

  if (paint_hover_effect) {
    SkPoint hover_location(gfx::PointToSkPoint(hover_controller_->location()));
    hover_location.scale(SkFloatToScalar(scale));
    const SkScalar kMinHoverRadius = 16;
     const SkScalar radius =
         std::max(SkFloatToScalar(tab_->width() / 4.f), kMinHoverRadius);
     DrawHighlight(canvas, hover_location, radius * scale,
                  SkColorSetA(active_color, hover_controller_->GetAlpha()));
   }
 }
