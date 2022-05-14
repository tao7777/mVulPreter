   static void GetCSI(const v8::FunctionCallbackInfo<v8::Value>& args) {
    args.GetReturnValue().SetNull();
     WebLocalFrame* frame = WebLocalFrame::frameForCurrentContext();
    if (!frame) {
      return;
     }
    WebDataSource* data_source = frame->dataSource();
    if (!data_source) {
      return;
    }
    DocumentState* document_state = DocumentState::FromDataSource(data_source);
    if (!document_state) {
      return;
    }
    base::Time now = base::Time::Now();
    base::Time start = document_state->request_time().is_null()
                           ? document_state->start_load_time()
                           : document_state->request_time();
    base::Time onload = document_state->finish_document_load_time();
    base::TimeDelta page = now - start;
    int navigation_type = GetCSITransitionType(data_source->navigationType());
    // Important: |frame|, |data_source| and |document_state| should not be
    // referred to below this line, as JS setters below can invalidate these
    // pointers.
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
    v8::Local<v8::Object> csi = v8::Object::New(isolate);
    if (!csi->Set(ctx, v8::String::NewFromUtf8(isolate, "startE",
                                               v8::NewStringType::kNormal)
                           .ToLocalChecked(),
                  v8::Number::New(isolate, floor(start.ToDoubleT() * 1000)))
             .FromMaybe(false)) {
      return;
    }
    if (!csi->Set(ctx, v8::String::NewFromUtf8(isolate, "onloadT",
                                               v8::NewStringType::kNormal)
                           .ToLocalChecked(),
                  v8::Number::New(isolate, floor(onload.ToDoubleT() * 1000)))
             .FromMaybe(false)) {
      return;
    }
    if (!csi->Set(ctx, v8::String::NewFromUtf8(isolate, "pageT",
                                               v8::NewStringType::kNormal)
                           .ToLocalChecked(),
                  v8::Number::New(isolate, page.InMillisecondsF()))
             .FromMaybe(false)) {
      return;
    }
    if (!csi->Set(ctx, v8::String::NewFromUtf8(isolate, "tran",
                                               v8::NewStringType::kNormal)
                           .ToLocalChecked(),
                  v8::Number::New(isolate, navigation_type))
             .FromMaybe(false)) {
      return;
    }
    args.GetReturnValue().Set(csi);
   }
