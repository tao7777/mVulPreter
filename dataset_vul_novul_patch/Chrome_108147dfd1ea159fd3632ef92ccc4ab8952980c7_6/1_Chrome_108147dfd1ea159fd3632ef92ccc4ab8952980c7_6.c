void FrameLoader::Trace(blink::Visitor* visitor) {
  visitor->Trace(frame_);
   visitor->Trace(progress_tracker_);
   visitor->Trace(document_loader_);
   visitor->Trace(provisional_document_loader_);
 }
