   static void GetCSI(const v8::FunctionCallbackInfo<v8::Value>& args) {
     WebLocalFrame* frame = WebLocalFrame::frameForCurrentContext();
    if (frame) {
      WebDataSource* data_source = frame->dataSource();
      if (data_source) {
        DocumentState* document_state =
            DocumentState::FromDataSource(data_source);
        v8::Isolate* isolate = args.GetIsolate();
        v8::Local<v8::Object> csi = v8::Object::New(isolate);
        base::Time now = base::Time::Now();
        base::Time start = document_state->request_time().is_null() ?
            document_state->start_load_time() :
            document_state->request_time();
        base::Time onload = document_state->finish_document_load_time();
        base::TimeDelta page = now - start;
        csi->Set(v8::String::NewFromUtf8(isolate, "startE"),
                 v8::Number::New(isolate, floor(start.ToDoubleT() * 1000)));
        csi->Set(v8::String::NewFromUtf8(isolate, "onloadT"),
                 v8::Number::New(isolate, floor(onload.ToDoubleT() * 1000)));
        csi->Set(v8::String::NewFromUtf8(isolate, "pageT"),
                 v8::Number::New(isolate, page.InMillisecondsF()));
        csi->Set(
            v8::String::NewFromUtf8(isolate, "tran"),
            v8::Number::New(
                isolate, GetCSITransitionType(data_source->navigationType())));
        args.GetReturnValue().Set(csi);
        return;
      }
     }
    args.GetReturnValue().SetNull();
    return;
   }
