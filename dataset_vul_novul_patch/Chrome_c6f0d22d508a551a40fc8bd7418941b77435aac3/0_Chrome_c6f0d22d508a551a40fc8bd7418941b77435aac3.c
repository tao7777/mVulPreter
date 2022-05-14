 bool OmniboxViewViews::ShouldShowPlaceholderText() const {
  bool show_with_caret = base::FeatureList::IsEnabled(
      omnibox::kUIExperimentShowPlaceholderWhenCaretShowing);
   return Textfield::ShouldShowPlaceholderText() &&
         (show_with_caret || !model()->is_caret_visible()) &&
         !model()->is_keyword_selected();
 }
