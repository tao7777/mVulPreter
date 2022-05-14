void RemoteFrame::ScheduleNavigation(Document& origin_document,
                                      const KURL& url,
                                      WebFrameLoadType frame_load_type,
                                      UserGestureStatus user_gesture_status) {
  if (!origin_document.GetSecurityOrigin()->CanDisplay(url)) {
    origin_document.AddConsoleMessage(ConsoleMessage::Create(
        kSecurityMessageSource, kErrorMessageLevel,
        "Not allowed to load local resource: " + url.ElidedString()));
    return;
  }

   FrameLoadRequest frame_request(&origin_document, ResourceRequest(url));
   frame_request.GetResourceRequest().SetHasUserGesture(
       user_gesture_status == UserGestureStatus::kActive);
  frame_request.GetResourceRequest().SetFrameType(
      IsMainFrame() ? network::mojom::RequestContextFrameType::kTopLevel
                    : network::mojom::RequestContextFrameType::kNested);
  Navigate(frame_request, frame_load_type);
}
