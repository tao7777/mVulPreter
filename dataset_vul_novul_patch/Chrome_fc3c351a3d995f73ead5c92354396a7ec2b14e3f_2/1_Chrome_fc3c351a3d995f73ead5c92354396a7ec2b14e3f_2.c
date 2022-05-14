void AfterTranslateInfoBar::Layout() {
   TranslateInfoBarBase::Layout();
 
  int available_width = InfoBar::GetAvailableWidth();
   gfx::Size pref_size = options_menu_button_->GetPreferredSize();
   options_menu_button_->SetBounds(available_width - pref_size.width(),
       OffsetY(this, pref_size), pref_size.width(), pref_size.height());

  views::MenuButton* left_button = swapped_language_buttons_ ?
      target_language_menu_button_ : original_language_menu_button_;
  views::MenuButton* right_button = swapped_language_buttons_ ?
       original_language_menu_button_ : target_language_menu_button_;
 
   pref_size = label_1_->GetPreferredSize();
  label_1_->SetBounds(icon_->bounds().right() + InfoBar::kIconLabelSpacing,
      InfoBar::OffsetY(this, pref_size), pref_size.width(), pref_size.height());
 
   pref_size = left_button->GetPreferredSize();
   left_button->SetBounds(label_1_->bounds().right() +
      InfoBar::kButtonInLabelSpacing, OffsetY(this, pref_size),
       pref_size.width(), pref_size.height());
 
   pref_size = label_2_->GetPreferredSize();
   label_2_->SetBounds(left_button->bounds().right() +
      InfoBar::kButtonInLabelSpacing, InfoBar::OffsetY(this, pref_size),
       pref_size.width(), pref_size.height());
 
   pref_size = right_button->GetPreferredSize();
   right_button->SetBounds(label_2_->bounds().right() +
      InfoBar::kButtonInLabelSpacing, OffsetY(this, pref_size),
       pref_size.width(), pref_size.height());
 
   pref_size = label_3_->GetPreferredSize();
   label_3_->SetBounds(right_button->bounds().right() +
      InfoBar::kButtonInLabelSpacing, InfoBar::OffsetY(this, pref_size),
       pref_size.width(), pref_size.height());
 
   pref_size = revert_button_->GetPreferredSize();
   revert_button_->SetBounds(label_3_->bounds().right() +
      InfoBar::kButtonInLabelSpacing, InfoBar::OffsetY(this, pref_size),
       pref_size.width(), pref_size.height());
 }
