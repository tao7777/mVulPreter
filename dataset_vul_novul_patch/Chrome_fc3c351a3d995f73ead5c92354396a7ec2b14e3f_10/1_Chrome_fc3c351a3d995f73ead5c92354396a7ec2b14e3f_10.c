 void InfoBarContainer::UpdateInfoBars() {
   for (int i = 0; i < tab_contents_->infobar_delegate_count(); ++i) {
     InfoBarDelegate* delegate = tab_contents_->GetInfoBarDelegateAt(i);
    InfoBar* infobar = delegate->CreateInfoBar();
     infobar->set_container(this);
     AddChildView(infobar);
     infobar->Open();
  }
}
