 void ShellWindowFrameView::OnPaint(gfx::Canvas* canvas) {
   SkPaint paint;
   paint.setAntiAlias(false);
  paint.setStyle(SkPaint::kFill_Style);
  paint.setColor(SK_ColorWHITE);
  gfx::Path path;
  const int radius = 1;
  path.moveTo(0, radius);
  path.lineTo(radius, 0);
  path.lineTo(width() - radius - 1, 0);
  path.lineTo(width(), radius + 1);
  path.lineTo(width(), kCaptionHeight);
  path.lineTo(0, kCaptionHeight);
  path.close();
  canvas->DrawPath(path, paint);
}
