void ImageBitmapFactories::Trace(blink::Visitor* visitor) {
  visitor->Trace(pending_loaders_);
  Supplement<LocalDOMWindow>::Trace(visitor);
  Supplement<WorkerGlobalScope>::Trace(visitor);
 }
