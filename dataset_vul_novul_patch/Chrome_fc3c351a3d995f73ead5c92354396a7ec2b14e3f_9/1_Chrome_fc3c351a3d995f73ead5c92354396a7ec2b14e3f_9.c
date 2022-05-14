void InfoBarContainer::RemoveInfoBar(InfoBarDelegate* delegate,
                                     bool use_animation) {
   for (int i = 0; i < GetChildViewCount(); ++i) {
    InfoBar* infobar = static_cast<InfoBar*>(GetChildViewAt(i));
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
