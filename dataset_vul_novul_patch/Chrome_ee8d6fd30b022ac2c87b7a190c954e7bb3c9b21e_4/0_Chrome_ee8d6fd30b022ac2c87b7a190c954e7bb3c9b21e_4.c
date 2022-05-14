 void DraggedTabGtk::Layout() {
   if (attached_) {
    renderer_->SetBounds(gfx::Rect(GetPreferredSize()));
   } else {
     int left = 0;
     if (base::i18n::IsRTL())
      left = GetPreferredSize().width() - attached_tab_size_.width();

    renderer_->SetBounds(gfx::Rect(left, 0, attached_tab_size_.width(),
                         attached_tab_size_.height()));
  }
}
