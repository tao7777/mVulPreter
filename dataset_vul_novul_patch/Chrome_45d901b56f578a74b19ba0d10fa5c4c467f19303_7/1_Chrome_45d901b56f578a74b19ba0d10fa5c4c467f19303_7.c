 void GM2TabStyle::PaintTabBackground(gfx::Canvas* canvas,
                                     bool active,
                                      int fill_id,
                                      int y_inset,
                                      const SkPath* clip) const {
   DCHECK(!y_inset || fill_id);
 
  const SkColor active_color =
      tab_->controller()->GetTabBackgroundColor(TAB_ACTIVE);
  const SkColor inactive_color =
      tab_->GetThemeProvider()->GetDisplayProperty(
          ThemeProperties::SHOULD_FILL_BACKGROUND_TAB_COLOR)
          ? tab_->controller()->GetTabBackgroundColor(TAB_INACTIVE)
          : SK_ColorTRANSPARENT;
   const SkColor stroke_color =
       tab_->controller()->GetToolbarTopSeparatorColor();
  const bool paint_hover_effect = !active && IsHoverActive();
  const float stroke_thickness = GetStrokeThickness(active);
 
  PaintTabBackgroundFill(canvas, active, paint_hover_effect, active_color,
                         inactive_color, fill_id, y_inset);
   if (stroke_thickness > 0) {
     gfx::ScopedCanvas scoped_canvas(clip ? canvas : nullptr);
     if (clip)
       canvas->sk_canvas()->clipPath(*clip, SkClipOp::kDifference, true);
    PaintBackgroundStroke(canvas, active, stroke_color);
   }
 
   PaintSeparators(canvas);
 }
