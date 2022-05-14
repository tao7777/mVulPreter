 void FrameSelection::DocumentAttached(Document* document) {
   DCHECK(document);
   selection_editor_->DocumentAttached(document);
   SetContext(document);
 }
