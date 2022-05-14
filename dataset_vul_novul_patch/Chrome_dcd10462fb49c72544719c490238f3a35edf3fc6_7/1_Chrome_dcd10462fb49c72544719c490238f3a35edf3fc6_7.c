void DistillerNativeJavaScript::AddJavaScriptObjectToFrame(
    v8::Local<v8::Context> context) {
  v8::Isolate* isolate = blink::mainThreadIsolate();
  v8::HandleScope handle_scope(isolate);
  if (context.IsEmpty())
    return;

  v8::Context::Scope context_scope(context);

   v8::Local<v8::Object> distiller_obj =
       GetOrCreateDistillerObject(isolate, context->Global());
 
   BindFunctionToObject(
       distiller_obj,
       "echo",
       base::Bind(
           &DistillerNativeJavaScript::DistillerEcho, base::Unretained(this)));
 
   BindFunctionToObject(
       distiller_obj,
       "sendFeedback",
       base::Bind(
          &DistillerNativeJavaScript::DistillerSendFeedback,
          base::Unretained(this)));
 
   BindFunctionToObject(
       distiller_obj,
       "closePanel",
       base::Bind(
          &DistillerNativeJavaScript::DistillerClosePanel,
          base::Unretained(this)));
 }
