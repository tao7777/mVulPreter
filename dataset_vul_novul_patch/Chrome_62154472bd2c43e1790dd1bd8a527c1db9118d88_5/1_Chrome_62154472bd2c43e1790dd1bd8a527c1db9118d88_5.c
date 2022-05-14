ScriptPromise Bluetooth::requestLEScan(ScriptState* script_state,
                                       const BluetoothLEScanOptions* options,
                                       ExceptionState& exception_state) {
  ExecutionContext* context = ExecutionContext::From(script_state);
  DCHECK(context);

  context->AddConsoleMessage(ConsoleMessage::Create(
      mojom::ConsoleMessageSource::kJavaScript,
      mojom::ConsoleMessageLevel::kInfo,
      "Web Bluetooth Scanning is experimental on this platform. See "
      "https://github.com/WebBluetoothCG/web-bluetooth/blob/gh-pages/"
      "implementation-status.md"));

  CHECK(context->IsSecureContext());

  auto& doc = *To<Document>(context);
  auto* frame = doc.GetFrame();
  if (!frame) {
    return ScriptPromise::Reject(
        script_state, V8ThrowException::CreateTypeError(
                          script_state->GetIsolate(), "Document not active"));
  }

  if (!LocalFrame::HasTransientUserActivation(frame)) {
    return ScriptPromise::RejectWithDOMException(
        script_state,
        MakeGarbageCollected<DOMException>(
            DOMExceptionCode::kSecurityError,
             "Must be handling a user gesture to show a permission request."));
   }
 
  if (!service_) {
    frame->GetInterfaceProvider().GetInterface(mojo::MakeRequest(
        &service_, context->GetTaskRunner(TaskType::kMiscPlatformAPI)));
  }
 
   auto scan_options = mojom::blink::WebBluetoothRequestLEScanOptions::New();
   ConvertRequestLEScanOptions(options, scan_options, exception_state);

  if (exception_state.HadException())
    return ScriptPromise();

  Platform::Current()->RecordRapporURL("Bluetooth.APIUsage.Origin", doc.Url());

  auto* resolver = MakeGarbageCollected<ScriptPromiseResolver>(script_state);
  ScriptPromise promise = resolver->Promise();

  mojom::blink::WebBluetoothScanClientAssociatedPtrInfo client;
  mojo::BindingId id = client_bindings_.AddBinding(
      this, mojo::MakeRequest(&client),
      context->GetTaskRunner(TaskType::kMiscPlatformAPI));

  service_->RequestScanningStart(
      std::move(client), std::move(scan_options),
      WTF::Bind(&Bluetooth::RequestScanningCallback, WrapPersistent(this),
                WrapPersistent(resolver), id));

  return promise;
}
