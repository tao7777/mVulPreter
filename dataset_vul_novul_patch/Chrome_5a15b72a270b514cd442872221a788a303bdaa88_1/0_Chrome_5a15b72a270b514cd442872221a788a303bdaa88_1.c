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
 
  v8::Global<v8::Function> v8_callback(context()->isolate(),
                                       args[1].As<v8::Function>());
  base::Callback<void(bool)> callback(
      base::Bind(&RenderFrameObserverNatives::InvokeCallback,
                 weak_ptr_factory_.GetWeakPtr(), base::Passed(&v8_callback)));
  if (ExtensionFrameHelper::Get(frame)->did_create_current_document_element()) {
    // If the document element is already created, then we can call the callback
    // immediately (though use PostTask to ensure that the callback is called
    // asynchronously).
    base::MessageLoop::current()->PostTask(FROM_HERE,
                                           base::Bind(callback, true));
  } else {
    new LoadWatcher(frame, callback);
  }
 
   args.GetReturnValue().Set(true);
 }
