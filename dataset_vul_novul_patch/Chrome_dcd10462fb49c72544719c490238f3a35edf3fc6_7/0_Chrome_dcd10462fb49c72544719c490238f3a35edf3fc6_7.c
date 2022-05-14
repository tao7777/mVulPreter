void DistillerNativeJavaScript::AddJavaScriptObjectToFrame(
    v8::Local<v8::Context> context) {
  v8::Isolate* isolate = blink::mainThreadIsolate();
  v8::HandleScope handle_scope(isolate);
  if (context.IsEmpty())
    return;

  v8::Context::Scope context_scope(context);

   v8::Local<v8::Object> distiller_obj =
       GetOrCreateDistillerObject(isolate, context->Global());
 
  EnsureServiceConnected();

  // Some of the JavaScript functions require extra work to be done when it is
  // called, so they have wrapper functions maintained in this class.
   BindFunctionToObject(
       distiller_obj,
       "echo",
       base::Bind(
           &DistillerNativeJavaScript::DistillerEcho, base::Unretained(this)));
 
  // Many functions can simply call the Mojo interface directly and have no
  // wrapper function for binding. Note that calling distiller_js_service.get()
  // does not transfer ownership of the interface.
   BindFunctionToObject(
       distiller_obj,
       "sendFeedback",
       base::Bind(
          &DistillerJavaScriptService::HandleDistillerFeedbackCall,
          base::Unretained(distiller_js_service_.get())));
 
   BindFunctionToObject(
       distiller_obj,
       "closePanel",
       base::Bind(
          &DistillerJavaScriptService::HandleDistillerClosePanelCall,
          base::Unretained(distiller_js_service_.get())));

  BindFunctionToObject(
      distiller_obj,
      "openSettings",
      base::Bind(
          &DistillerJavaScriptService::HandleDistillerOpenSettingsCall,
          base::Unretained(distiller_js_service_.get())));
 }
