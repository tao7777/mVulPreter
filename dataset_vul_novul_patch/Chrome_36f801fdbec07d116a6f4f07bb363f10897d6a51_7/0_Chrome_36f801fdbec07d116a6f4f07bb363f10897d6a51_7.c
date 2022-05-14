static Frame* ReuseExistingWindow(LocalFrame& active_frame,
                                  LocalFrame& lookup_frame,
                                  const AtomicString& frame_name,
                                  NavigationPolicy policy,
                                  const KURL& destination_url) {
  if (!frame_name.IsEmpty() && !EqualIgnoringASCIICase(frame_name, "_blank") &&
      policy == kNavigationPolicyIgnore) {
    if (Frame* frame = lookup_frame.FindFrameForNavigation(
            frame_name, active_frame, destination_url)) {
      if (!EqualIgnoringASCIICase(frame_name, "_self")) {
        if (Page* page = frame->GetPage()) {
           if (page == active_frame.GetPage())
             page->GetFocusController().SetFocusedFrame(frame);
           else
            page->GetChromeClient().Focus(&active_frame);
         }
       }
       return frame;
    }
  }
  return nullptr;
}
