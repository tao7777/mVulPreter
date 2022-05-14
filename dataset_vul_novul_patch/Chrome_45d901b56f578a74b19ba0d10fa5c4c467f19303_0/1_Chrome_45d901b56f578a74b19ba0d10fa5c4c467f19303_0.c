 void TabGroupHeader::OnPaint(gfx::Canvas* canvas) {
  constexpr SkColor kPlaceholderColor = SkColorSetRGB(0xAA, 0xBB, 0xCC);
   gfx::Rect fill_bounds(GetLocalBounds());
   fill_bounds.Inset(TabStyle::GetTabOverlap(), 0);
  canvas->FillRect(fill_bounds, kPlaceholderColor);
 }
