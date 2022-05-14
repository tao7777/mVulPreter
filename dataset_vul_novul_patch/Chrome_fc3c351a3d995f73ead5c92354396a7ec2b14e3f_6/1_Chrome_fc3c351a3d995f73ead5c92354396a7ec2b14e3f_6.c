void ExtensionInfoBar::OnExtensionPreferredSizeChanged(ExtensionView* view) {
  DCHECK(view == delegate_->extension_host()->view());

  if (delegate_->closing())
    return;

  delegate_->extension_host()->view()->SetVisible(true);
 
   gfx::Size sz = view->GetPreferredSize();
  int default_height = static_cast<int>(InfoBar::kDefaultTargetHeight);
   sz.set_height(std::max(default_height, sz.height()));
   sz.set_height(std::min(2 * default_height, sz.height()));
 
  if (height() == 0)
    animation()->Reset(0.0);
  set_target_height(sz.height());
  animation()->Show();
}
