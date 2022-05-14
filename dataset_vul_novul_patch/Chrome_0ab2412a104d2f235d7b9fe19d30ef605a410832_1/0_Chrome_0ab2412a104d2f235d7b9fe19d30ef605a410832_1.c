 void WebLocalFrameImpl::LoadJavaScriptURL(const KURL& url) {
   DCHECK(GetFrame());

  Document* owner_document = GetFrame()->GetDocument();

  if (!owner_document || !GetFrame()->GetPage())
    return;

  if (SchemeRegistry::ShouldTreatURLSchemeAsNotAllowingJavascriptURLs(
          owner_document->Url().Protocol()))
    return;

  String script = DecodeURLEscapeSequences(
      url.GetString().Substring(strlen("javascript:")));
  UserGestureIndicator gesture_indicator(
      UserGestureToken::Create(owner_document, UserGestureToken::kNewGesture));
  v8::HandleScope handle_scope(ToIsolate(GetFrame()));
  v8::Local<v8::Value> result =
      GetFrame()->GetScriptController().ExecuteScriptInMainWorldAndReturnValue(
          ScriptSourceCode(script));
  if (result.IsEmpty() || !result->IsString())
    return;
  String script_result = ToCoreString(v8::Local<v8::String>::Cast(result));
  if (!GetFrame()->GetNavigationScheduler().LocationChangePending()) {
    GetFrame()->Loader().ReplaceDocumentWhileExecutingJavaScriptURL(
        script_result, owner_document);
  }
}
