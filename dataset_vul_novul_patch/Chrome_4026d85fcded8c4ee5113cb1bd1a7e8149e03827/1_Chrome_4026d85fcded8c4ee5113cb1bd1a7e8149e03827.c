   static void GetLoadTimes(const v8::FunctionCallbackInfo<v8::Value>& args) {
     WebLocalFrame* frame = WebLocalFrame::frameForCurrentContext();
    if (frame) {
      WebDataSource* data_source = frame->dataSource();
      if (data_source) {
        DocumentState* document_state =
            DocumentState::FromDataSource(data_source);
        v8::Isolate* isolate = args.GetIsolate();
        v8::Local<v8::Object> load_times = v8::Object::New(isolate);
        load_times->Set(
            v8::String::NewFromUtf8(isolate, "requestTime"),
            v8::Number::New(isolate,
                            document_state->request_time().ToDoubleT()));
        load_times->Set(
            v8::String::NewFromUtf8(isolate, "startLoadTime"),
            v8::Number::New(isolate,
                            document_state->start_load_time().ToDoubleT()));
        load_times->Set(
            v8::String::NewFromUtf8(isolate, "commitLoadTime"),
            v8::Number::New(isolate,
                            document_state->commit_load_time().ToDoubleT()));
        load_times->Set(
            v8::String::NewFromUtf8(isolate, "finishDocumentLoadTime"),
            v8::Number::New(
                isolate,
                document_state->finish_document_load_time().ToDoubleT()));
        load_times->Set(
            v8::String::NewFromUtf8(isolate, "finishLoadTime"),
            v8::Number::New(isolate,
                            document_state->finish_load_time().ToDoubleT()));
        load_times->Set(
            v8::String::NewFromUtf8(isolate, "firstPaintTime"),
            v8::Number::New(isolate,
                            document_state->first_paint_time().ToDoubleT()));
        load_times->Set(
            v8::String::NewFromUtf8(isolate, "firstPaintAfterLoadTime"),
            v8::Number::New(
                isolate,
                document_state->first_paint_after_load_time().ToDoubleT()));
        load_times->Set(
            v8::String::NewFromUtf8(isolate, "navigationType"),
            v8::String::NewFromUtf8(
                isolate, GetNavigationType(data_source->navigationType())));
        load_times->Set(
            v8::String::NewFromUtf8(isolate, "wasFetchedViaSpdy"),
            v8::Boolean::New(isolate, document_state->was_fetched_via_spdy()));
        load_times->Set(
            v8::String::NewFromUtf8(isolate, "wasNpnNegotiated"),
            v8::Boolean::New(isolate, document_state->was_npn_negotiated()));
        load_times->Set(
            v8::String::NewFromUtf8(isolate, "npnNegotiatedProtocol"),
            v8::String::NewFromUtf8(
                isolate, document_state->npn_negotiated_protocol().c_str()));
        load_times->Set(
            v8::String::NewFromUtf8(isolate, "wasAlternateProtocolAvailable"),
            v8::Boolean::New(
                isolate, document_state->was_alternate_protocol_available()));
        load_times->Set(v8::String::NewFromUtf8(isolate, "connectionInfo"),
                        v8::String::NewFromUtf8(
                            isolate,
                            net::HttpResponseInfo::ConnectionInfoToString(
                                document_state->connection_info()).c_str()));
        args.GetReturnValue().Set(load_times);
        return;
      }
     }
    args.GetReturnValue().SetNull();
   }
