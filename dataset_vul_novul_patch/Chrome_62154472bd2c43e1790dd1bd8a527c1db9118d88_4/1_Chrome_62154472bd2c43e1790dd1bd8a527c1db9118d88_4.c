ScriptPromise Bluetooth::requestDevice(ScriptState* script_state,
                                       const RequestDeviceOptions* options,
                                       ExceptionState& exception_state) {
  ExecutionContext* context = ExecutionContext::From(script_state);

#if !defined(OS_CHROMEOS) && !defined(OS_ANDROID) && !defined(OS_MACOSX) && \
    !defined(OS_WIN)
  context->AddConsoleMessage(ConsoleMessage::Create(
      mojom::ConsoleMessageSource::kJavaScript,
      mojom::ConsoleMessageLevel::kInfo,
      "Web Bluetooth is experimental on this platform. See "
      "https://github.com/WebBluetoothCG/web-bluetooth/blob/gh-pages/"
      "implementation-status.md"));
#endif

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
 
  auto device_options = mojom::blink::WebBluetoothRequestDeviceOptions::New();
  ConvertRequestDeviceOptions(options, device_options, exception_state);

  if (exception_state.HadException())
    return ScriptPromise();

  Platform::Current()->RecordRapporURL("Bluetooth.APIUsage.Origin", doc.Url());

  auto* resolver = MakeGarbageCollected<ScriptPromiseResolver>(script_state);
  ScriptPromise promise = resolver->Promise();

  service_->RequestDevice(
      std::move(device_options),
      WTF::Bind(&Bluetooth::RequestDeviceCallback, WrapPersistent(this),
                WrapPersistent(resolver)));
  return promise;
}
