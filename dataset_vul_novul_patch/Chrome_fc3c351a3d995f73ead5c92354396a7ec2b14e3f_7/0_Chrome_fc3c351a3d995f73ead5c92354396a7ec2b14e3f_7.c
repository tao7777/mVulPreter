 void InfoBarContainer::AddInfoBar(InfoBarDelegate* delegate,
                                   bool use_animation) {
  InfoBarView* infobar = static_cast<InfoBarView*>(delegate->CreateInfoBar());
   infobar->set_container(this);
   AddChildView(infobar);
 
  if (use_animation)
    infobar->AnimateOpen();
  else
    infobar->Open();
}
