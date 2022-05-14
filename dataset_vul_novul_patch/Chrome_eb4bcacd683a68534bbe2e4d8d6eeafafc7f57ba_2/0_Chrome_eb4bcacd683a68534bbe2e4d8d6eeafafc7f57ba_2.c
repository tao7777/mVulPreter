v8::Handle<v8::Value> AppWindowCustomBindings::GetView(
    const v8::Arguments& args) {
  if (args.Length() != 1)
    return v8::Undefined();

  if (!args[0]->IsInt32())
    return v8::Undefined();

  int view_id = args[0]->Int32Value();

  if (view_id == MSG_ROUTING_NONE)
    return v8::Undefined();

  FindViewByID view_finder(view_id);
  content::RenderView::ForEach(&view_finder);
  content::RenderView* view = view_finder.view();
  if (!view)
    return v8::Undefined();

  content::RenderView* render_view = GetCurrentRenderView();
  if (!render_view)
    return v8::Undefined();
   WebKit::WebFrame* opener = render_view->GetWebView()->mainFrame();
   WebKit::WebFrame* frame = view->GetWebView()->mainFrame();
   frame->setOpener(opener);
  content::RenderThread::Get()->Send(
      new ExtensionHostMsg_ResumeRequests(view->GetRoutingID()));
 
   v8::Local<v8::Value> window = frame->mainWorldScriptContext()->Global();
   return window;
}
