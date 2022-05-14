 void ExtensionInfoBar::Layout() {
  InfoBarView::Layout();
 
   int x = 0;
  gfx::Size sz = menu_->GetPreferredSize();
  menu_->SetBounds(x,
                  (height() - sz.height()) / 2,
                   sz.width(), sz.height());
  x += sz.width() + kMenuHorizontalMargin;

  ExtensionView* view = delegate_->extension_host()->view();
  view->SetBounds(x, 0, width() - x - kFarRightMargin - 1, height() - 1);
}
