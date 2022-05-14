static ScriptPromise fulfillImageBitmap(ExecutionContext* context, PassRefPtrWillBeRawPtr<ImageBitmap> imageBitmap)
 {
     RefPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(context);
     ScriptPromise promise = resolver->promise();
    resolver->resolve(imageBitmap);
     return promise;
 }
