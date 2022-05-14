void WebContentsImpl::OnOpenColorChooser(
      int color_chooser_id,
      SkColor color,
       const std::vector<ColorSuggestion>& suggestions) {
   ColorChooser* new_color_chooser =
       delegate_->OpenColorChooser(this, color, suggestions);
  if (!new_color_chooser)
     return;
  if (color_chooser_)
    color_chooser_->End();
   color_chooser_.reset(new_color_chooser);
   color_chooser_identifier_ = color_chooser_id;
 }
