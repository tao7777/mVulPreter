   static void GetLoadTimes(const v8::FunctionCallbackInfo<v8::Value>& args) {
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
     double request_time = document_state->request_time().ToDoubleT();
    double start_load_time = document_state->start_load_time().ToDoubleT();
    double commit_load_time = document_state->commit_load_time().ToDoubleT();
    double finish_document_load_time =
        document_state->finish_document_load_time().ToDoubleT();
    double finish_load_time = document_state->finish_load_time().ToDoubleT();
    double first_paint_time = document_state->first_paint_time().ToDoubleT();
    double first_paint_after_load_time =
        document_state->first_paint_after_load_time().ToDoubleT();
    std::string navigation_type =
        GetNavigationType(data_source->navigationType());
    bool was_fetched_via_spdy = document_state->was_fetched_via_spdy();
    bool was_npn_negotiated = document_state->was_npn_negotiated();
    std::string npn_negotiated_protocol =
        document_state->npn_negotiated_protocol();
    bool was_alternate_protocol_available =
        document_state->was_alternate_protocol_available();
    std::string connection_info = net::HttpResponseInfo::ConnectionInfoToString(
        document_state->connection_info());
     v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
     v8::Local<v8::Object> load_times = v8::Object::New(isolate);
    if (!load_times
             ->Set(ctx, v8::String::NewFromUtf8(isolate, "requestTime",
                                                v8::NewStringType::kNormal)
                            .ToLocalChecked(),
                   v8::Number::New(isolate, request_time))
             .FromMaybe(false)) {
      return;
    }

    if (!load_times
             ->Set(ctx, v8::String::NewFromUtf8(isolate, "startLoadTime",
                                                v8::NewStringType::kNormal)
                            .ToLocalChecked(),
                   v8::Number::New(isolate, start_load_time))
             .FromMaybe(false)) {
      return;
    }
    if (!load_times
             ->Set(ctx, v8::String::NewFromUtf8(isolate, "commitLoadTime",
                                                v8::NewStringType::kNormal)
                            .ToLocalChecked(),
                   v8::Number::New(isolate, commit_load_time))
             .FromMaybe(false)) {
      return;
    }
    if (!load_times
             ->Set(ctx,
                   v8::String::NewFromUtf8(isolate, "finishDocumentLoadTime",
                                           v8::NewStringType::kNormal)
                       .ToLocalChecked(),
                   v8::Number::New(isolate, finish_document_load_time))
             .FromMaybe(false)) {
      return;
    }
    if (!load_times
             ->Set(ctx, v8::String::NewFromUtf8(isolate, "finishLoadTime",
                                                v8::NewStringType::kNormal)
                            .ToLocalChecked(),
                   v8::Number::New(isolate, finish_load_time))
             .FromMaybe(false)) {
      return;
    }
    if (!load_times
             ->Set(ctx, v8::String::NewFromUtf8(isolate, "firstPaintTime",
                                                v8::NewStringType::kNormal)
                            .ToLocalChecked(),
                   v8::Number::New(isolate, first_paint_time))
             .FromMaybe(false)) {
      return;
    }
    if (!load_times
             ->Set(ctx,
                   v8::String::NewFromUtf8(isolate, "firstPaintAfterLoadTime",
                                           v8::NewStringType::kNormal)
                       .ToLocalChecked(),
                   v8::Number::New(isolate, first_paint_after_load_time))
             .FromMaybe(false)) {
      return;
    }
    if (!load_times
             ->Set(ctx, v8::String::NewFromUtf8(isolate, "navigationType",
                                                v8::NewStringType::kNormal)
                            .ToLocalChecked(),
                   v8::String::NewFromUtf8(isolate, navigation_type.c_str(),
                                           v8::NewStringType::kNormal)
                       .ToLocalChecked())
             .FromMaybe(false)) {
      return;
    }
    if (!load_times
             ->Set(ctx, v8::String::NewFromUtf8(isolate, "wasFetchedViaSpdy",
                                                v8::NewStringType::kNormal)
                            .ToLocalChecked(),
                   v8::Boolean::New(isolate, was_fetched_via_spdy))
             .FromMaybe(false)) {
      return;
    }
    if (!load_times
             ->Set(ctx, v8::String::NewFromUtf8(isolate, "wasNpnNegotiated",
                                                v8::NewStringType::kNormal)
                            .ToLocalChecked(),
                   v8::Boolean::New(isolate, was_npn_negotiated))
             .FromMaybe(false)) {
      return;
    }
    if (!load_times
             ->Set(ctx,
                   v8::String::NewFromUtf8(isolate, "npnNegotiatedProtocol",
                                           v8::NewStringType::kNormal)
                       .ToLocalChecked(),
                   v8::String::NewFromUtf8(isolate,
                                           npn_negotiated_protocol.c_str(),
                                           v8::NewStringType::kNormal)
                       .ToLocalChecked())
             .FromMaybe(false)) {
      return;
    }
    if (!load_times
             ->Set(ctx, v8::String::NewFromUtf8(isolate,
                                                "wasAlternateProtocolAvailable",
                                                v8::NewStringType::kNormal)
                            .ToLocalChecked(),
                   v8::Boolean::New(isolate, was_alternate_protocol_available))
             .FromMaybe(false)) {
      return;
    }
    if (!load_times
             ->Set(ctx, v8::String::NewFromUtf8(isolate, "connectionInfo",
                                                v8::NewStringType::kNormal)
                            .ToLocalChecked(),
                   v8::String::NewFromUtf8(isolate, connection_info.c_str(),
                                           v8::NewStringType::kNormal)
                       .ToLocalChecked())
             .FromMaybe(false)) {
      return;
    }
     args.GetReturnValue().Set(load_times);
   }
