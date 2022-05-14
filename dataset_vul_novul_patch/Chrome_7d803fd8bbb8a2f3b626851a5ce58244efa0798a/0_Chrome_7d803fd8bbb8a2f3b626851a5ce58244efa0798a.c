DOMWindow* CreateWindow(const String& url_string,
                        const AtomicString& frame_name,
                        const String& window_features_string,
                        LocalDOMWindow& calling_window,
                        LocalFrame& first_frame,
                        LocalFrame& opener_frame,
                        ExceptionState& exception_state) {
  LocalFrame* active_frame = calling_window.GetFrame();
  DCHECK(active_frame);

  KURL completed_url = url_string.IsEmpty()
                           ? KURL(kParsedURLString, g_empty_string)
                           : first_frame.GetDocument()->CompleteURL(url_string);
  if (!completed_url.IsEmpty() && !completed_url.IsValid()) {
    UseCounter::Count(active_frame, WebFeature::kWindowOpenWithInvalidURL);
    exception_state.ThrowDOMException(
        kSyntaxError, "Unable to open a window with invalid URL '" +
                          completed_url.GetString() + "'.\n");
     return nullptr;
   }
 
  if (completed_url.ProtocolIsJavaScript() &&
      opener_frame.GetDocument()->GetContentSecurityPolicy() &&
      !ContentSecurityPolicy::ShouldBypassMainWorld(
          opener_frame.GetDocument())) {
    const int kJavascriptSchemeLength = sizeof("javascript:") - 1;
    String script_source = DecodeURLEscapeSequences(completed_url.GetString())
                               .Substring(kJavascriptSchemeLength);

    if (!opener_frame.GetDocument()
             ->GetContentSecurityPolicy()
             ->AllowJavaScriptURLs(nullptr, script_source,
                                   opener_frame.GetDocument()->Url(),
                                   OrdinalNumber())) {
      return nullptr;
    }
  }

   WebWindowFeatures window_features =
       GetWindowFeaturesFromString(window_features_string);
 
  FrameLoadRequest frame_request(calling_window.document(),
                                 ResourceRequest(completed_url), frame_name);
  frame_request.SetShouldSetOpener(window_features.noopener ? kNeverSetOpener
                                                            : kMaybeSetOpener);
  frame_request.GetResourceRequest().SetFrameType(
      WebURLRequest::kFrameTypeAuxiliary);
  frame_request.GetResourceRequest().SetRequestorOrigin(
      SecurityOrigin::Create(active_frame->GetDocument()->Url()));

  frame_request.GetResourceRequest().SetHTTPReferrer(
      SecurityPolicy::GenerateReferrer(
          active_frame->GetDocument()->GetReferrerPolicy(), completed_url,
          active_frame->GetDocument()->OutgoingReferrer()));

  bool has_user_gesture = UserGestureIndicator::ProcessingUserGesture();

  bool created;
  Frame* new_frame = CreateWindowHelper(
      opener_frame, *active_frame, opener_frame, frame_request, window_features,
      kNavigationPolicyIgnore, created);
  if (!new_frame)
    return nullptr;
  if (new_frame->DomWindow()->IsInsecureScriptAccess(calling_window,
                                                     completed_url))
    return window_features.noopener ? nullptr : new_frame->DomWindow();

  if (created) {
    FrameLoadRequest request(calling_window.document(),
                             ResourceRequest(completed_url));
    request.GetResourceRequest().SetHasUserGesture(has_user_gesture);
    new_frame->Navigate(request);
  } else if (!url_string.IsEmpty()) {
    new_frame->Navigate(*calling_window.document(), completed_url, false,
                        has_user_gesture ? UserGestureStatus::kActive
                                         : UserGestureStatus::kNone);
  }
  return window_features.noopener ? nullptr : new_frame->DomWindow();
}
