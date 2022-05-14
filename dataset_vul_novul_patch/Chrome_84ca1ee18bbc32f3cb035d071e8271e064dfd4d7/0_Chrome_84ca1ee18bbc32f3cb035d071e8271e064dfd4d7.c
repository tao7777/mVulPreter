 void ImageCapture::ResolveWithMediaTrackConstraints(
    ScriptValue constraints,
     ScriptPromiseResolver* resolver) {
   DCHECK(resolver);
   resolver->Resolve(constraints);
}
