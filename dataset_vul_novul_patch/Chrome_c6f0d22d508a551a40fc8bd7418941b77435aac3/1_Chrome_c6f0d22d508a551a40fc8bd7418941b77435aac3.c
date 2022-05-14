 bool OmniboxViewViews::ShouldShowPlaceholderText() const {
   return Textfield::ShouldShowPlaceholderText() &&
         !model()->is_caret_visible() && !model()->is_keyword_selected();
 }
