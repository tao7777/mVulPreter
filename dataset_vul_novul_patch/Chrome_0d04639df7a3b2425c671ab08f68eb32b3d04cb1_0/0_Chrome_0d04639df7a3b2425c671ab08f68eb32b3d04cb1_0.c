void AutoFillCCInfoBarDelegate::InfoBarClosed() {
  if (host_) {
     host_->OnInfoBarClosed(false);
     host_ = NULL;
   }
  delete this;
 }
