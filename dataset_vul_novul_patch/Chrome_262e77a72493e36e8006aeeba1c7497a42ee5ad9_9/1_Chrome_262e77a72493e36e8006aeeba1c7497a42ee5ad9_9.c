 ScriptPromise VRDisplay::exitPresent(ScriptState* script_state) {
   ScriptPromiseResolver* resolver = ScriptPromiseResolver::Create(script_state);
   ScriptPromise promise = resolver->Promise();
 
  if (!is_presenting_) {
    DOMException* exception = DOMException::Create(
        kInvalidStateError, "VRDisplay is not presenting.");
    resolver->Reject(exception);
    return promise;
  }

  if (!display_) {
    DOMException* exception =
        DOMException::Create(kInvalidStateError, "VRService is not available.");
    resolver->Reject(exception);
    return promise;
  }
  display_->ExitPresent();

  resolver->Resolve();

  StopPresenting();

  return promise;
}
