 void TranslateInfoBarBase::Layout() {
  InfoBar::Layout();
 
   gfx::Size icon_ps = icon_->GetPreferredSize();
  icon_->SetBounds(InfoBar::kHorizontalPadding, InfoBar::OffsetY(this, icon_ps),
      icon_ps.width(), icon_ps.height());
 }
