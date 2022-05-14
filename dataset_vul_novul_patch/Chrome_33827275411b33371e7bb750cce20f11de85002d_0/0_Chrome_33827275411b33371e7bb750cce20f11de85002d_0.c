void FrameSelection::Clear() {
  granularity_ = TextGranularity::kCharacter;
   if (granularity_strategy_)
     granularity_strategy_->Clear();
   SetSelection(SelectionInDOMTree());
  is_handle_visible_ = false;
 }
