ConfirmInfoBar::ConfirmInfoBar(std::unique_ptr<ConfirmInfoBarDelegate> delegate)
     : InfoBarView(std::move(delegate)) {
   auto* delegate_ptr = GetDelegate();
   label_ = CreateLabel(delegate_ptr->GetMessageText());
  label_->SetElideBehavior(delegate_ptr->GetMessageElideBehavior());
   AddChildView(label_);
 
   const auto buttons = delegate_ptr->GetButtons();
  if (buttons & ConfirmInfoBarDelegate::BUTTON_OK) {
    ok_button_ = CreateButton(ConfirmInfoBarDelegate::BUTTON_OK);
    ok_button_->SetProminent(true);
    if (delegate_ptr->OKButtonTriggersUACPrompt()) {
      elevation_icon_setter_.reset(new ElevationIconSetter(
          ok_button_,
          base::BindOnce(&ConfirmInfoBar::Layout, base::Unretained(this))));
    }
  }

  if (buttons & ConfirmInfoBarDelegate::BUTTON_CANCEL) {
    cancel_button_ = CreateButton(ConfirmInfoBarDelegate::BUTTON_CANCEL);
    if (buttons == ConfirmInfoBarDelegate::BUTTON_CANCEL)
      cancel_button_->SetProminent(true);
  }

  link_ = CreateLink(delegate_ptr->GetLinkText(), this);
  AddChildView(link_);
}
