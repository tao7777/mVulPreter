  LoadWatcher(ScriptContext* context,
  LoadWatcher(content::RenderFrame* frame,
              const base::Callback<void(bool)>& callback)
      : content::RenderFrameObserver(frame), callback_(callback) {}

  void DidCreateDocumentElement() override {
    // The callback must be run as soon as the root element is available.
    // Running the callback may trigger DidCreateDocumentElement or
    // DidFailProvisionalLoad, so delete this before running the callback.
    base::Callback<void(bool)> callback = callback_;
    delete this;
    callback.Run(true);
   }
