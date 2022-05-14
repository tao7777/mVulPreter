 RenderFrameObserverNatives::RenderFrameObserverNatives(ScriptContext* context)
    : ObjectBackedNativeHandler(context), weak_ptr_factory_(this) {
   RouteFunction(
       "OnDocumentElementCreated",
       base::Bind(&RenderFrameObserverNatives::OnDocumentElementCreated,
                  base::Unretained(this)));
 }
