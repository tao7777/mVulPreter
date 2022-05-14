void BeforeTranslateInfoBar::Layout() {
   TranslateInfoBarBase::Layout();
 
  int available_width = InfoBarView::GetAvailableWidth();
   gfx::Size pref_size = options_menu_button_->GetPreferredSize();
   options_menu_button_->SetBounds(available_width - pref_size.width(),
       OffsetY(this, pref_size), pref_size.width(), pref_size.height());
 
   pref_size = label_1_->GetPreferredSize();
  label_1_->SetBounds(icon_->bounds().right() + InfoBarView::kIconLabelSpacing,
                      InfoBarView::OffsetY(this, pref_size), pref_size.width(),
                      pref_size.height());
 
   pref_size = language_menu_button_->GetPreferredSize();
   language_menu_button_->SetBounds(label_1_->bounds().right() +
      InfoBarView::kButtonInLabelSpacing, OffsetY(this, pref_size),
       pref_size.width(), pref_size.height());
 
   pref_size = label_2_->GetPreferredSize();
   label_2_->SetBounds(language_menu_button_->bounds().right() +
      InfoBarView::kButtonInLabelSpacing, InfoBarView::OffsetY(this, pref_size),
       pref_size.width(), pref_size.height());
 
   pref_size = accept_button_->GetPreferredSize();
   accept_button_->SetBounds(
      label_2_->bounds().right() + InfoBarView::kEndOfLabelSpacing,
       OffsetY(this, pref_size), pref_size.width(), pref_size.height());
 
   pref_size = deny_button_->GetPreferredSize();
   deny_button_->SetBounds(
        accept_button_->bounds().right() + InfoBarView::kButtonButtonSpacing,
         OffsetY(this, pref_size), pref_size.width(), pref_size.height());
 
   if (never_translate_button_) {
     pref_size = never_translate_button_->GetPreferredSize();
     never_translate_button_->SetBounds(
          deny_button_->bounds().right() + InfoBarView::kButtonButtonSpacing,
           OffsetY(this, pref_size), pref_size.width(), pref_size.height());
   }
   if (always_translate_button_) {
     DCHECK(!never_translate_button_);
     pref_size = always_translate_button_->GetPreferredSize();
     always_translate_button_->SetBounds(
          deny_button_->bounds().right() + InfoBarView::kButtonButtonSpacing,
           OffsetY(this, pref_size), pref_size.width(), pref_size.height());
   }
 }
