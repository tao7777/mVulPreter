void InputConnectionImpl::CommitText(const base::string16& text,
                                     int new_cursor_pos) {
  StartStateUpdateTimer();

  std::string error;
  if (!ime_engine_->ClearComposition(input_context_id_, &error))
    LOG(ERROR) << "ClearComposition failed: error=\"" << error << "\"";

  if (IsControlChar(text)) {
    SendControlKeyEvent(text);
    return;
  }

   if (!ime_engine_->CommitText(input_context_id_,
                                base::UTF16ToUTF8(text).c_str(), &error))
     LOG(ERROR) << "CommitText failed: error=\"" << error << "\"";
  composing_text_.clear();
 }
