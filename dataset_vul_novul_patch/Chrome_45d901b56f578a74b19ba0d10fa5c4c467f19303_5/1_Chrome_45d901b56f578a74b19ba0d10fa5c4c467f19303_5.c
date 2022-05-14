 void GM2TabStyle::PaintInactiveTabBackground(gfx::Canvas* canvas,
                                              const SkPath& clip) const {
   bool has_custom_image;
   int fill_id = tab_->controller()->GetBackgroundResourceId(&has_custom_image);
   if (!has_custom_image)
     fill_id = 0;
 
  PaintTabBackground(canvas, false /* active */, fill_id, 0,
                      tab_->controller()->MaySetClip() ? &clip : nullptr);
 }
