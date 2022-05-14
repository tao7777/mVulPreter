void InfoBarContainer::RemoveInfoBar(InfoBarDelegate* delegate,
                                     bool use_animation) {
   for (int i = 0; i < GetChildViewCount(); ++i) {
    InfoBarView* infobar = static_cast<InfoBarView*>(GetChildViewAt(i));
     if (infobar->delegate() == delegate) {
       if (use_animation) {
        infobar->AnimateClose();
      } else {
        infobar->Close();
      }
      break;
    }
  }
}
