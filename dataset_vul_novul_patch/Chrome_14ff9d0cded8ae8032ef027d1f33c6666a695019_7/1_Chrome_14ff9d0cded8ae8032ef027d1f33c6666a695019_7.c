 PageCaptureCustomBindings::PageCaptureCustomBindings(ScriptContext* context)
     : ObjectBackedNativeHandler(context) {
  RouteFunction("CreateBlob",
      base::Bind(&PageCaptureCustomBindings::CreateBlob,
                 base::Unretained(this)));
  RouteFunction("SendResponseAck",
      base::Bind(&PageCaptureCustomBindings::SendResponseAck,
                 base::Unretained(this)));
 }
