 void DraggedTabGtk::Layout() {
   if (attached_) {
    gfx::Size prefsize = GetPreferredSize();
    renderer_->SetBounds(gfx::Rect(0, 0, prefsize.width(), prefsize.height()));
   } else {
     int left = 0;
     if (base::i18n::IsRTL())
      left = GetPreferredSize().width() - attached_tab_size_.width();

    renderer_->SetBounds(gfx::Rect(left, 0, attached_tab_size_.width(),
                         attached_tab_size_.height()));
  }
}
