 PageCaptureCustomBindings::PageCaptureCustomBindings(ScriptContext* context)
     : ObjectBackedNativeHandler(context) {
  RouteFunction("CreateBlob", "pageCapture",
                base::Bind(&PageCaptureCustomBindings::CreateBlob,
                           base::Unretained(this)));
  RouteFunction("SendResponseAck", "pageCapture",
                base::Bind(&PageCaptureCustomBindings::SendResponseAck,
                           base::Unretained(this)));
 }
