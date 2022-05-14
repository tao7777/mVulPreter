 void TranslateMessageInfoBar::Layout() {
   TranslateInfoBarBase::Layout();
 
  int x = icon_->bounds().right() + InfoBar::kIconLabelSpacing;
   gfx::Size label_pref_size = label_->GetPreferredSize();
   int available_width = GetAvailableWidth() - x;
   gfx::Size button_pref_size;
   if (button_) {
     button_pref_size = button_->GetPreferredSize();
     available_width -=
        (button_pref_size.width() + InfoBar::kButtonInLabelSpacing);
   }
  label_->SetBounds(x, InfoBar::OffsetY(this, label_pref_size),
                     std::min(label_pref_size.width(), available_width),
                     label_pref_size.height());
 
   if (button_) {
     button_->SetBounds(label_->bounds().right() +
                          InfoBar::kButtonInLabelSpacing,
                       InfoBar::OffsetY(this, button_pref_size),
                        button_pref_size.width(), button_pref_size.height());
   }
 }
