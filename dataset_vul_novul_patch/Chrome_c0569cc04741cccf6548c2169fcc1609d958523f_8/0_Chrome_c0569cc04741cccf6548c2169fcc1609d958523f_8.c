 RenderFrameObserverNatives::RenderFrameObserverNatives(ScriptContext* context)
     : ObjectBackedNativeHandler(context), weak_ptr_factory_(this) {
   RouteFunction(
      "OnDocumentElementCreated", "app.window",
       base::Bind(&RenderFrameObserverNatives::OnDocumentElementCreated,
                  base::Unretained(this)));
 }
