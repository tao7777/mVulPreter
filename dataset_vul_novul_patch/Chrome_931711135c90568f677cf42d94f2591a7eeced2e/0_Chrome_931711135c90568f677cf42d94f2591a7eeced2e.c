void Document::open(Document* entered_document,
                    ExceptionState& exception_state) {
  if (ImportLoader()) {
    exception_state.ThrowDOMException(
        kInvalidStateError, "Imported document doesn't support open().");
    return;
  }

  if (!IsHTMLDocument()) {
    exception_state.ThrowDOMException(kInvalidStateError,
                                      "Only HTML documents support open().");
    return;
  }

  if (throw_on_dynamic_markup_insertion_count_) {
    exception_state.ThrowDOMException(
        kInvalidStateError,
        "Custom Element constructor should not use open().");
    return;
  }

  if (entered_document) {
    if (!GetSecurityOrigin()->IsSameSchemeHostPortAndSuborigin(
            entered_document->GetSecurityOrigin())) {
      exception_state.ThrowSecurityError(
          "Can only call open() on same-origin documents.");
      return;
    }
    SetSecurityOrigin(entered_document->GetSecurityOrigin());

    if (this != entered_document) {
       KURL new_url = entered_document->Url();
       new_url.SetFragmentIdentifier(String());
       SetURL(new_url);
      SetReferrerPolicy(entered_document->GetReferrerPolicy());
     }
 
     cookie_url_ = entered_document->CookieURL();
  }

  open();
}
