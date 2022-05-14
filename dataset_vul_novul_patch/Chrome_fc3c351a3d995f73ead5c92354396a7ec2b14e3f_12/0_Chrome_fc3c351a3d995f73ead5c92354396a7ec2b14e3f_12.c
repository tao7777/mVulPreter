 void TranslateInfoBarBase::Layout() {
  InfoBarView::Layout();
 
   gfx::Size icon_ps = icon_->GetPreferredSize();
  icon_->SetBounds(InfoBarView::kHorizontalPadding,
      InfoBarView::OffsetY(this, icon_ps), icon_ps.width(), icon_ps.height());
 }
