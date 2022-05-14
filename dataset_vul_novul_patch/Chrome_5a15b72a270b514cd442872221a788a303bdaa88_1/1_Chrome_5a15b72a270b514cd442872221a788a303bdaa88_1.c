 void RenderFrameObserverNatives::OnDocumentElementCreated(
     const v8::FunctionCallbackInfo<v8::Value>& args) {
   CHECK(args.Length() == 2);
  CHECK(args[0]->IsInt32());
  CHECK(args[1]->IsFunction());

  int frame_id = args[0]->Int32Value();

  content::RenderFrame* frame = content::RenderFrame::FromRoutingID(frame_id);
  if (!frame) {
    LOG(WARNING) << "No render frame found to register LoadWatcher.";
     return;
   }
 
  new LoadWatcher(context(), frame, args[1].As<v8::Function>());
 
   args.GetReturnValue().Set(true);
 }
