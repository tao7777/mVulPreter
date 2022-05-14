  LoadWatcher(ScriptContext* context,
              content::RenderFrame* frame,
              v8::Local<v8::Function> cb)
      : content::RenderFrameObserver(frame),
        context_(context),
        callback_(context->isolate(), cb) {
    if (ExtensionFrameHelper::Get(frame)->
            did_create_current_document_element()) {
      base::MessageLoop::current()->PostTask(
          FROM_HERE,
          base::Bind(&LoadWatcher::CallbackAndDie, base::Unretained(this),
                     true));
    }
   }
