 void InfoBarContainer::AddInfoBar(InfoBarDelegate* delegate,
                                   bool use_animation) {
  InfoBar* infobar = delegate->CreateInfoBar();
   infobar->set_container(this);
   AddChildView(infobar);
 
  if (use_animation)
    infobar->AnimateOpen();
  else
    infobar->Open();
}
