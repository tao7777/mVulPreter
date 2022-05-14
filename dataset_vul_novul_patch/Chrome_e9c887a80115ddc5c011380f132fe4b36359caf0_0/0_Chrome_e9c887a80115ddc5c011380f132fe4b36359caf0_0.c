static ScriptPromise fulfillImageBitmap(ExecutionContext* context, PassRefPtrWillBeRawPtr<ImageBitmap> imageBitmap)
 {
     RefPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(context);
     ScriptPromise promise = resolver->promise();
    if (imageBitmap) {
        resolver->resolve(imageBitmap);
    } else {
        v8::Isolate* isolate = ScriptState::current()->isolate();
        resolver->reject(ScriptValue(v8::Null(isolate), isolate));
    }
     return promise;
 }
