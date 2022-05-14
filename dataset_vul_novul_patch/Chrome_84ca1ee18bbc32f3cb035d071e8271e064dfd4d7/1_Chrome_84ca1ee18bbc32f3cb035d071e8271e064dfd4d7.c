 void ImageCapture::ResolveWithMediaTrackConstraints(
    MediaTrackConstraints constraints,
     ScriptPromiseResolver* resolver) {
   DCHECK(resolver);
   resolver->Resolve(constraints);
}
