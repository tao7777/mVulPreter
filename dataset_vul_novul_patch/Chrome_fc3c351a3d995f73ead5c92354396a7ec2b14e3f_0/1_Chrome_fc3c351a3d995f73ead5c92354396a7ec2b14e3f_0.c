 void SaveCCInfoConfirmInfoBar::Layout() {
  InfoBar::Layout();
 
   int available_width = AlertInfoBar::GetAvailableWidth();
 
  link_->SetVisible(!link_->GetText().empty());
  gfx::Size link_ps = link_->GetPreferredSize();
  int link_x = available_width - kButtonButtonSpacing - link_ps.width();
  link_->SetBounds(link_x, OffsetY(this, link_ps), link_ps.width(),
                   link_ps.height());
  available_width = link_x;

  gfx::Size save_ps = save_button_->GetPreferredSize();
  gfx::Size dont_save_ps = dont_save_button_->GetPreferredSize();

  AlertInfoBar::Layout();

  int save_x = label()->bounds().right() + kEndOfLabelSpacing;
  save_x = std::max(0, std::min(save_x, available_width - (save_ps.width() +
                    dont_save_ps.width() + kButtonButtonSpacing)));

  save_button_->SetVisible(true);
  dont_save_button_->SetVisible(true);

  save_button_->SetBounds(save_x, OffsetY(this, save_ps), save_ps.width(),
                          save_ps.height());
  int dont_save_x = save_x + save_ps.width() + kButtonButtonSpacing;
  dont_save_button_->SetBounds(dont_save_x, OffsetY(this, dont_save_ps),
                               dont_save_ps.width(), dont_save_ps.height());
}
