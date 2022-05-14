 void GM2TabStyle::PaintTabBackground(gfx::Canvas* canvas,
                                     TabState active_state,
                                      int fill_id,
                                      int y_inset,
                                      const SkPath* clip) const {
   DCHECK(!y_inset || fill_id);
 
   const SkColor stroke_color =
       tab_->controller()->GetToolbarTopSeparatorColor();
  const bool paint_hover_effect =
      active_state == TAB_INACTIVE && IsHoverActive();
  const float stroke_thickness = GetStrokeThickness(active_state == TAB_ACTIVE);
 
  PaintTabBackgroundFill(canvas, active_state, paint_hover_effect, fill_id,
                         y_inset);
   if (stroke_thickness > 0) {
     gfx::ScopedCanvas scoped_canvas(clip ? canvas : nullptr);
     if (clip)
       canvas->sk_canvas()->clipPath(*clip, SkClipOp::kDifference, true);
    PaintBackgroundStroke(canvas, active_state, stroke_color);
   }
 
   PaintSeparators(canvas);
 }
