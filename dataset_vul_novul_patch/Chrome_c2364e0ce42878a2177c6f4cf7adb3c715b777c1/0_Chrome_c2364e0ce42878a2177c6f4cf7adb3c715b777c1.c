 void OmniboxEditModel::RestoreState(const State* state) {
  bool url_replacement_enabled = !state || state->url_replacement_enabled;
   controller_->GetToolbarModel()->set_url_replacement_enabled(
      url_replacement_enabled);
  controller_->GetToolbarModel()->set_origin_chip_enabled(
      url_replacement_enabled);
   permanent_text_ = controller_->GetToolbarModel()->GetText();
  view_->RevertWithoutResettingSearchTermReplacement();
  input_ = state ? state->autocomplete_input : AutocompleteInput();
  if (!state)
    return;

  SetFocusState(state->focus_state, OMNIBOX_FOCUS_CHANGE_TAB_SWITCH);
  focus_source_ = state->focus_source;
  if (state->user_input_in_progress) {
    keyword_ = state->keyword;
    is_keyword_hint_ = state->is_keyword_hint;
    view_->SetUserText(state->user_text,
        DisplayTextFromUserText(state->user_text), false);
    view_->SetGrayTextAutocompletion(state->gray_text);
  }
}
