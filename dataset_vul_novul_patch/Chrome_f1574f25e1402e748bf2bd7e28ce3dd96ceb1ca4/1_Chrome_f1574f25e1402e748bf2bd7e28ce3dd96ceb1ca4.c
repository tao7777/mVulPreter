 void FrameSelection::DocumentAttached(Document* document) {
   DCHECK(document);
  use_secure_keyboard_entry_when_active_ = false;
   selection_editor_->DocumentAttached(document);
   SetContext(document);
 }
