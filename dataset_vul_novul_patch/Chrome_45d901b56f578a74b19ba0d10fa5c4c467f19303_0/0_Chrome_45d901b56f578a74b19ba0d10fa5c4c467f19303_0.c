 void TabGroupHeader::OnPaint(gfx::Canvas* canvas) {
   gfx::Rect fill_bounds(GetLocalBounds());
   fill_bounds.Inset(TabStyle::GetTabOverlap(), 0);
  const SkColor color = GetGroupData()->color();
  canvas->FillRect(fill_bounds, color);
  title_label_->SetBackgroundColor(color);
}

const TabGroupData* TabGroupHeader::GetGroupData() {
  return controller_->GetDataForGroup(group_);
 }
