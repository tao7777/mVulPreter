void RuntimeCustomBindings::GetExtensionViews(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  if (args.Length() != 2)
    return;

  if (!args[0]->IsInt32() || !args[1]->IsString())
    return;

  int browser_window_id = args[0]->Int32Value();

  std::string view_type_string =
      base::ToUpperASCII(*v8::String::Utf8Value(args[1]));
  ViewType view_type = VIEW_TYPE_INVALID;
  if (view_type_string == kViewTypeBackgroundPage) {
    view_type = VIEW_TYPE_EXTENSION_BACKGROUND_PAGE;
  } else if (view_type_string == kViewTypeTabContents) {
    view_type = VIEW_TYPE_TAB_CONTENTS;
  } else if (view_type_string == kViewTypePopup) {
    view_type = VIEW_TYPE_EXTENSION_POPUP;
  } else if (view_type_string == kViewTypeExtensionDialog) {
    view_type = VIEW_TYPE_EXTENSION_DIALOG;
  } else if (view_type_string == kViewTypeAppWindow) {
    view_type = VIEW_TYPE_APP_WINDOW;
  } else if (view_type_string == kViewTypeLauncherPage) {
    view_type = VIEW_TYPE_LAUNCHER_PAGE;
  } else if (view_type_string == kViewTypePanel) {
    view_type = VIEW_TYPE_PANEL;
  } else if (view_type_string != kViewTypeAll) {
    return;
  }

  std::string extension_id = context()->GetExtensionID();
  if (extension_id.empty())
    return;

   std::vector<content::RenderFrame*> frames =
       ExtensionFrameHelper::GetExtensionFrames(extension_id, browser_window_id,
                                                view_type);
  v8::Local<v8::Context> v8_context = args.GetIsolate()->GetCurrentContext();
   v8::Local<v8::Array> v8_views = v8::Array::New(args.GetIsolate());
   int v8_index = 0;
   for (content::RenderFrame* frame : frames) {
    if (frame->GetWebFrame()->top() != frame->GetWebFrame())
      continue;

    v8::Local<v8::Context> context =
        frame->GetWebFrame()->mainWorldScriptContext();
     if (!context.IsEmpty()) {
       v8::Local<v8::Value> window = context->Global();
       DCHECK(!window.IsEmpty());
      v8::Maybe<bool> maybe =
        v8_views->CreateDataProperty(v8_context, v8_index++, window);
      DCHECK(maybe.IsJust() && maybe.FromJust());
     }
   }
 
  args.GetReturnValue().Set(v8_views);
}
