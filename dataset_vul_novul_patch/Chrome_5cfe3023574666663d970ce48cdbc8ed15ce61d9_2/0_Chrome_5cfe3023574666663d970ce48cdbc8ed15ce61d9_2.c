 gfx::Size CardUnmaskPromptViews::GetPreferredSize() const {
  // Must hardcode a width so the label knows where to wrap.
   const int kWidth = 375;
   return gfx::Size(kWidth, GetHeightForWidth(kWidth));
 }
