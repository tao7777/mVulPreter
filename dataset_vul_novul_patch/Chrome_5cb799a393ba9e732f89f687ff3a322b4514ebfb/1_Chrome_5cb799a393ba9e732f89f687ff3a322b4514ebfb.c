static bool ShouldAutofocus(const HTMLFormControlElement* element) {
  if (!element->isConnected())
    return false;
  if (!element->IsAutofocusable())
    return false;



   Document& doc = element->GetDocument();
 
 
  if (doc.IsSandboxed(WebSandboxFlags::kAutomaticFeatures)) {
    doc.AddConsoleMessage(ConsoleMessage::Create(
        mojom::ConsoleMessageSource::kSecurity,
        mojom::ConsoleMessageLevel::kError,
        "Blocked autofocusing on a form control because the form's frame is "
        "sandboxed and the 'allow-scripts' permission is not set."));
    return false;
  }

  if (!doc.IsInMainFrame() &&
      !doc.TopFrameOrigin()->CanAccess(doc.GetSecurityOrigin())) {
    doc.AddConsoleMessage(ConsoleMessage::Create(
        mojom::ConsoleMessageSource::kSecurity,
        mojom::ConsoleMessageLevel::kError,
        "Blocked autofocusing on a form control in a cross-origin subframe."));
    return false;
  }

  return true;
}
