void VRDisplay::ProcessScheduledWindowAnimations(double timestamp) {
  TRACE_EVENT1("gpu", "VRDisplay::window.rAF", "frame", vr_frame_id_);
  auto doc = navigator_vr_->GetDocument();
  if (!doc)
    return;
   auto page = doc->GetPage();
   if (!page)
     return;
   page->Animator().ServiceScriptedAnimations(timestamp);
 }
