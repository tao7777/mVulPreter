 void GM2TabStyle::PaintBackgroundStroke(gfx::Canvas* canvas,
                                        bool active,
                                         SkColor stroke_color) const {
   SkPath outer_path =
      GetPath(TabStyle::PathType::kBorder, canvas->image_scale(), active);
   gfx::ScopedCanvas scoped_canvas(canvas);
   float scale = canvas->UndoDeviceScaleFactor();
   cc::PaintFlags flags;
   flags.setAntiAlias(true);
   flags.setColor(stroke_color);
   flags.setStyle(cc::PaintFlags::kStroke_Style);
  flags.setStrokeWidth(GetStrokeThickness(active) * scale);
   canvas->DrawPath(outer_path, flags);
 }
