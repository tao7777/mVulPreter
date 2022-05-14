v8::Handle<v8::Value> V8AudioContext::constructorCallback(const v8::Arguments& args)
{
    INC_STATS("DOM.AudioContext.Contructor");

    if (!args.IsConstructCall())
        return V8Proxy::throwTypeError("AudioContext constructor cannot be called as a function.");

    if (ConstructorMode::current() == ConstructorMode::WrapExistingObject)
        return args.Holder();

    Frame* frame = V8Proxy::retrieveFrameForCurrentContext();
    if (!frame)
        return V8Proxy::throwError(V8Proxy::ReferenceError, "AudioContext constructor associated frame is unavailable", args.GetIsolate());

    Document* document = frame->document();
    if (!document)
        return V8Proxy::throwError(V8Proxy::ReferenceError, "AudioContext constructor associated document is unavailable", args.GetIsolate());

    RefPtr<AudioContext> audioContext;
    
    if (!args.Length()) {
        ExceptionCode ec = 0;
        audioContext = AudioContext::create(document, ec);
        if (ec)
            return throwError(ec, args.GetIsolate());
        if (!audioContext.get())
            return V8Proxy::throwError(V8Proxy::SyntaxError, "audio resources unavailable for AudioContext construction", args.GetIsolate());
    } else {
         if (args.Length() < 3)
            return V8Proxy::throwNotEnoughArgumentsError(args.GetIsolate());
 
         bool ok = false;
 
        int32_t numberOfChannels = toInt32(args[0], ok);
        if (!ok || numberOfChannels <= 0 || numberOfChannels > 10)
            return V8Proxy::throwError(V8Proxy::SyntaxError, "Invalid number of channels", args.GetIsolate());

        int32_t numberOfFrames = toInt32(args[1], ok);
        if (!ok || numberOfFrames <= 0)
            return V8Proxy::throwError(V8Proxy::SyntaxError, "Invalid number of frames", args.GetIsolate());

        float sampleRate = toFloat(args[2]);
        if (sampleRate <= 0)
            return V8Proxy::throwError(V8Proxy::SyntaxError, "Invalid sample rate", args.GetIsolate());

        ExceptionCode ec = 0;
        audioContext = AudioContext::createOfflineContext(document, numberOfChannels, numberOfFrames, sampleRate, ec);
        if (ec)
            return throwError(ec, args.GetIsolate());
    }

    if (!audioContext.get())
        return V8Proxy::throwError(V8Proxy::SyntaxError, "Error creating AudioContext", args.GetIsolate());
    
    V8DOMWrapper::setDOMWrapper(args.Holder(), &info, audioContext.get());
    audioContext->ref();
    
    return args.Holder();
}
