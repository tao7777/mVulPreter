 void ImageView::Paint(gfx::Canvas* canvas) {
   View::Paint(canvas);
 
   gfx::Rect image_bounds(GetImageBounds());
   if (image_bounds.IsEmpty())
     return;

  if (image_bounds.size() != gfx::Size(image_.width(), image_.height())) {
    image_.buildMipMap(false);
    SkPaint paint;
    paint.setFilterBitmap(true);
    canvas->DrawBitmapInt(image_, 0, 0, image_.width(), image_.height(),
        image_bounds.x(), image_bounds.y(), image_bounds.width(),
        image_bounds.height(), true, paint);
  } else {
    canvas->DrawBitmapInt(image_, image_bounds.x(), image_bounds.y());
  }
}
