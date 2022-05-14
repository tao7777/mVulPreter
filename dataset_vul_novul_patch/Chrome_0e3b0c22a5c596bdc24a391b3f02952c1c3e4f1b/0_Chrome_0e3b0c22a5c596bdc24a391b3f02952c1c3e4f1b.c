void Location::SetLocation(const String& url,
                           LocalDOMWindow* current_window,
                           LocalDOMWindow* entered_window,
                           ExceptionState* exception_state,
                           SetLocationPolicy set_location_policy) {
  if (!IsAttached())
    return;

  if (!current_window->GetFrame())
    return;

  Document* entered_document = entered_window->document();
  if (!entered_document)
    return;

  KURL completed_url = entered_document->CompleteURL(url);
  if (completed_url.IsNull())
    return;

  if (!current_window->GetFrame()->CanNavigate(*dom_window_->GetFrame(),
                                               completed_url)) {
    if (exception_state) {
      exception_state->ThrowSecurityError(
          "The current window does not have permission to navigate the target "
          "frame to '" +
          url + "'.");
    }
    return;
  }
  if (exception_state && !completed_url.IsValid()) {
    exception_state->ThrowDOMException(DOMExceptionCode::kSyntaxError,
                                       "'" + url + "' is not a valid URL.");
    return;
  }

   if (dom_window_->IsInsecureScriptAccess(*current_window, completed_url))
     return;
 
  // Check the source browsing context's CSP to fulfill the CSP check
  // requirement of https://html.spec.whatwg.org/#navigate for javascript URLs.
  // Although the spec states we should perform this check on task execution,
  // we do this prior to dispatch since the parent frame's CSP may be
  // inaccessible if the target frame is out of process.
  Document* current_document = current_window->document();
  if (current_document && completed_url.ProtocolIsJavaScript() &&
      !ContentSecurityPolicy::ShouldBypassMainWorld(current_document)) {
    String script_source = DecodeURLEscapeSequences(completed_url.GetString());
    if (!current_document->GetContentSecurityPolicy()->AllowJavaScriptURLs(
            nullptr, script_source, current_document->Url(), OrdinalNumber())) {
      return;
    }
  }

   V8DOMActivityLogger* activity_logger =
       V8DOMActivityLogger::CurrentActivityLoggerIfIsolatedWorld();
   if (activity_logger) {
    Vector<String> argv;
    argv.push_back("LocalDOMWindow");
    argv.push_back("url");
    argv.push_back(entered_document->Url());
    argv.push_back(completed_url);
    activity_logger->LogEvent("blinkSetAttribute", argv.size(), argv.data());
  }
  WebFrameLoadType frame_load_type = WebFrameLoadType::kStandard;
  if (set_location_policy == SetLocationPolicy::kReplaceThisFrame)
    frame_load_type = WebFrameLoadType::kReplaceCurrentItem;
  dom_window_->GetFrame()->ScheduleNavigation(*current_window->document(),
                                              completed_url, frame_load_type,
                                              UserGestureStatus::kNone);
}
