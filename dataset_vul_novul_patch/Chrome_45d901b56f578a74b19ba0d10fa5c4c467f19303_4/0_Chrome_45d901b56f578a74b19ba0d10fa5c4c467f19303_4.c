 void GM2TabStyle::PaintBackgroundStroke(gfx::Canvas* canvas,
                                        TabState active_state,
                                         SkColor stroke_color) const {
   SkPath outer_path =
      GetPath(TabStyle::PathType::kBorder, canvas->image_scale(),
              active_state == TAB_ACTIVE);
   gfx::ScopedCanvas scoped_canvas(canvas);
   float scale = canvas->UndoDeviceScaleFactor();
   cc::PaintFlags flags;
   flags.setAntiAlias(true);
   flags.setColor(stroke_color);
   flags.setStyle(cc::PaintFlags::kStroke_Style);
  flags.setStrokeWidth(GetStrokeThickness(active_state == TAB_ACTIVE) * scale);
   canvas->DrawPath(outer_path, flags);
 }
